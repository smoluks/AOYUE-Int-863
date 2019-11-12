using System;
using IRHeaterHardwareLibrary.Data;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Threading.Tasks;
using System.Threading;

namespace IRHeaterHardwareLibrary.Controllers
{
    internal class ComPortDispatcher
    {
        private SerialPort _serialPort;

        internal async Task<ErrorEnum> Connect(string comPort, bool autoPort)
        {
            if (!autoPort && comPort != null)
            {
                //-----использовать заданный вручную порт----
                var openPortResult = TryOpenPort(comPort);
                _serialPort = openPortResult.port;

                return openPortResult.error;
            }

            //-----поиск порта-----
            string[] ports = SerialPort.GetPortNames();
            if (ports.Length == 0)
                return ErrorEnum.NoPorts;

            var checkedPortsList = new List<SerialPort>();

            //список задач на проверку портов
            var currentTaskList = new List<Task<(SerialPort port, ErrorEnum error)>>(ports.Length);
            currentTaskList.AddRange(ports.Select(CheckPortAsync));

            while (currentTaskList.Count > 0)
            {
                //Ожидаем завершение любой задачи
                Task<(SerialPort port, ErrorEnum error)> finishedTask = await Task.WhenAny(currentTaskList);

                //Удаляем задачу из списка текущих
                currentTaskList.Remove(finishedTask);

                //Получаем результат завершенной
                var result = await finishedTask;
                if (result.error == ErrorEnum.NoError)
                    checkedPortsList.Add(result.port);
            }

            //если не найдено ни одного подходящего порта, возврат
            if (checkedPortsList.Count == 0)
                return ErrorEnum.PortNotFound;

            //выбираем для использования один
            _serialPort = checkedPortsList[0];

            //закрываем остальные
            for (var i = 1; i < checkedPortsList.Count; i++)
                checkedPortsList[i].Close();

            return ErrorEnum.NoError;
        }

        internal Task Disconnect()
        {
            return Task.Run(() =>
            {
                if (_serialPort != null && _serialPort.IsOpen)
                {
                    _serialPort.Close();
                }
            });
        }

        readonly object _lock = new object();

        internal Task<byte[]> ExchangePacketAsync(byte[] packet)
        {
            if (packet.Length > 255)
                throw new HardwareLibraryException($"Packet size must be < 256, but {packet.Length}");

            //подготовка данных к отправке
            byte[] data = new byte[packet.Length + 3];
            data[0] = 0xAE;
            data[1] = (byte)packet.Length;
            packet.CopyTo(data, 2);
            data[data.Length - 1] = GetCRC(packet);

            return Task.Run(() =>
            {
                int count = 0;
                do
                {
                    try
                    {
                        lock (_lock)
                        {
                            if (_serialPort.BytesToRead > 0)
                            {
                                string lostdata = _serialPort.ReadExisting();
                            }
                            //
                            _serialPort.Write(data, 0, data.Length);
                            //receive
                            byte b = (byte)_serialPort.ReadByte();
                            if (b == 0xAB)
                                throw new ApplicationException("Bad channel");
                            if (b == 0xAC)
                                throw  new ApplicationException("Unsupported command");
                            if (b == 0xAD)
                                throw new ApplicationException("Bad length");
                            if (b != 0xAE)
                                break;
                            int length = _serialPort.ReadByte();
                            byte[] receivePacket = new byte[length];

                            while (_serialPort.BytesToRead < length) { }
                            _serialPort.Read(receivePacket, 0, length);

                            byte crc = (byte)_serialPort.ReadByte();
                            //TODO: check crc

                            return receivePacket;
                        }
                    }
                    catch (TimeoutException)
                    {
                        //если операция не прошла, потупить и повторить
                        Thread.Sleep(100);
                    }
                }
                while (count++ < 5);
                return null;
            });
        }

        private Task<(SerialPort port, ErrorEnum error)> CheckPortAsync(string comPort)
        {
            return Task.Run(() => TryOpenPort(comPort));
        }

        private (SerialPort port, ErrorEnum error) TryOpenPort(string port)
        {
            try
            {
                var serialPort = new SerialPort(port, 38400)
                {
                    ReadTimeout = 2000,
                    WriteTimeout = 2000
                };

                serialPort.Open();
                serialPort.Write(new byte[] { 0xA8 }, 0, 1);
                int data = serialPort.ReadByte();
                if (data != 0xA8)
                {
                    serialPort.Close();
                    return (port: null, error: ErrorEnum.NoSlave);
                }
                return (port: serialPort, error: ErrorEnum.NoError);
            }
            catch (IOException)
            {
                return (port: null, error: ErrorEnum.PortIoError);
            }
            catch (TimeoutException)
            {
                return (port: null, error: ErrorEnum.PortTimeoutError);
            }
            catch (UnauthorizedAccessException)
            {
                return (port: null, error: ErrorEnum.PortUnauthorizedAccessError);
            }
            catch
            {
                return (port: null, error: ErrorEnum.UnknownError);
            }
        }

        public static byte GetCRC(IList<byte> data)
        {
            byte crc = 0;
            foreach (byte t in data)
            {
                var currentByte = t;
                for (byte bitCounter = 0; bitCounter < 8; bitCounter++)
                {
                    if (((crc ^ currentByte) & 0x01) != 0)
                    {
                        crc = (byte)((crc >> 1) ^ 0x8C);
                    }
                    else
                    {
                        crc >>= 1;
                    }
                    currentByte >>= 1;
                }
            }
            return crc;
        }
    }
}
