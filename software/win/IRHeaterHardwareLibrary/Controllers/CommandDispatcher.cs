using System;
using System.Threading.Tasks;
using IRHeaterHardwareLibrary.Data;
using IRHeaterHardwareLibrary.Entities;

namespace IRHeaterHardwareLibrary.Controllers
{
    internal static class CommandDispatcher
    {
        private static Configuration _configuration;

        internal static async Task<ErrorEnum> ConnectAsync(string comPort, bool autoPort)
        {
            ErrorEnum error = await PacketDispatcher.ConnectAsync(comPort, autoPort);

            if(error == ErrorEnum.NoError)
                _configuration = await PacketDispatcher.ReadConfiguration();

            return error;
        }

        internal static async Task DisconnectAsync()
        {
            await PacketDispatcher.DisconnectAsync();
        }

        internal static Configuration GetConfiguration()
        {
            return _configuration;
        }

        internal static async Task<Temperatures> GetTemperaturesAsync()
        {
            return await PacketDispatcher.GetTemperaturesAsync();
        }
        internal static async Task SetTargetAsync(ModeEnum mode, float[] temperatures)
        {
            if(_configuration.Sensors.Length != temperatures.Length)
                throw new ApplicationException($"Error: target channels {_configuration.Sensors.Length}, but config channels {temperatures.Length}");

            await PacketDispatcher.SetTargetAsync(mode, temperatures);
        }
    }
}
