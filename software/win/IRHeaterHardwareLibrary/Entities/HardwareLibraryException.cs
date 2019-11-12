using System;
using System.Runtime.Serialization;

namespace IRHeaterHardwareLibrary.Data
{
    public class HardwareLibraryException : ApplicationException
    {
        public HardwareLibraryException() { }

        public HardwareLibraryException(string message) : base(message) { }

        public HardwareLibraryException(string message, Exception inner) : base(message, inner) { }

        protected HardwareLibraryException(SerializationInfo info, StreamingContext context) : base(info, context) { }
    }
}
