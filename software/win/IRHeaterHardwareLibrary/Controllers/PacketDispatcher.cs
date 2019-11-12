using IRHeaterHardwareLibrary.Data;
using IRHeaterHardwareLibrary.Entities;
using System;
using System.Threading.Tasks;

namespace IRHeaterHardwareLibrary.Controllers
{
    static class PacketDispatcher
    {
        private static readonly byte[] getMainNamePacket = {0x00, 0x00};

        private static readonly byte[] getSensorCountPacket = { 0x01, 0xFF};
        private static readonly byte[] getAllValues = { 0x01, 0x00, 0x10 };

        private static readonly byte[] getChannelCountPacket = { 0x02, 0xFF};

        static ComPortDispatcher comPortDispatcher = new ComPortDispatcher();

        internal static async Task<ErrorEnum> ConnectAsync(string comPort, bool autoPort)
        {
            return await comPortDispatcher.Connect(comPort, autoPort);
        }

        internal static async Task DisconnectAsync()
        {
            await comPortDispatcher.Disconnect();
        }

        internal static async Task<Configuration> ReadConfiguration()
        {
            var configuration = new Configuration
            {
                MainName = ByteArrayToString(await comPortDispatcher.ExchangePacketAsync(getMainNamePacket))
            };
            //main
            //sensor
            byte sensorCount = GetByte(await comPortDispatcher.ExchangePacketAsync(getSensorCountPacket));
            configuration.Sensors = new string[sensorCount];
            for(int i  = 1; i <= sensorCount; i++)
            {
                configuration.Sensors[i-1] = ByteArrayToString(await comPortDispatcher.ExchangePacketAsync(new byte[] { 0x01, (byte)i, 0x0 }));
            }
            //channel
            byte channelCount = GetByte(await comPortDispatcher.ExchangePacketAsync(getChannelCountPacket));
            configuration.Channels = new string[channelCount];
            for (int i = 1; i <= channelCount; i++)
            {
                configuration.Channels[i - 1] = ByteArrayToString(await comPortDispatcher.ExchangePacketAsync(new byte[] { 0x02, (byte)i, 0x0 }));
            }
            //
            return configuration;
        }

        internal static async Task SetTargetAsync(ModeEnum mode, float[] temperatures)
        {
            byte[] packet = new byte[4 + 2 * temperatures.Length];
            packet[0] = 0x02;
            packet[1] = 0x00;
            packet[2] = 0x20;
            packet[3] = (byte)mode;
            for (int i = 0; i < temperatures.Length; i++)
            {
                short temp = (short) Math.Round(temperatures[i] * 16);
                packet[i * 2 + 5] = (byte)(temp >> 8);
                packet[i * 2 + 4] = (byte) (temp & 0xFF);
            }
            await comPortDispatcher.ExchangePacketAsync(packet);
        }

        internal static async Task<Temperatures> GetTemperaturesAsync()
        {
            var currentTemperatures = new Temperatures {Time = DateTime.Now};

            byte[] rawData = await comPortDispatcher.ExchangePacketAsync(getAllValues);
            currentTemperatures.temperature = new float[rawData.Length/2];
            for (int i = 0; i < rawData.Length; i += 2)
            {
                currentTemperatures.temperature[i / 2] = GetTemperature(rawData[i + 1], rawData[i]);
            }

            return currentTemperatures;
        }

        private static float GetTemperature(byte highByte, byte lowByte)
        {
            float temp = (highByte << 8) + lowByte;
            return temp / 16;
        }

        private static string ByteArrayToString(byte[] data)
        {
            if (data == null)
                return "";
            return System.Text.Encoding.UTF8.GetString(data, 0, data.Length);
        }
        private static byte GetByte(byte[] data)
        {
            if (data == null)
                return 0;

            return data[0];
        }
    }
}
