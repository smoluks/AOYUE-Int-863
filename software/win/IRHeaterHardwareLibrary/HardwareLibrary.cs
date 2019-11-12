using System.Threading.Tasks;
using IRHeaterHardwareLibrary.Controllers;
using IRHeaterHardwareLibrary.Data;
using IRHeaterHardwareLibrary.Entities;

namespace IRHeaterHardwareLibrary
{
    /// <summary>
    /// Основной класс библиотеки и внешний контракт
    /// </summary>
    public class HardwareLibrary
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="comPort"></param>
        /// <param name="autoPort"></param>
        /// <returns></returns>
        public async Task<ErrorEnum> ConnectAsync(string comPort, bool autoPort)
        {
            return await CommandDispatcher.ConnectAsync(comPort, autoPort);
        }

        /// <summary>
        /// 
        /// </summary>
        public async void DisconnectAsync()
        {
            await CommandDispatcher.DisconnectAsync();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public async Task<Temperatures> GetTemperaturesAsync()
        {
            return await CommandDispatcher.GetTemperaturesAsync();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public Configuration GetConfiguration()
        {
            return CommandDispatcher.GetConfiguration();
        }

        /// <summary>
        /// 
        /// </summary>
        public async Task SetTargetAsync(ModeEnum mode, float[] temperatures)
        {
            await CommandDispatcher.SetTargetAsync(mode, temperatures);
        }

    ~HardwareLibrary()
        {
            CommandDispatcher.DisconnectAsync();
        }
    }
}
