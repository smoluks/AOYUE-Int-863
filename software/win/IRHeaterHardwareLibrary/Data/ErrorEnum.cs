namespace IRHeaterHardwareLibrary.Data
{
    public enum ErrorEnum
    {
        NoError,
        NoPorts,
        PortNotFound,
        NoSlave,
        PortIOError,
        UnknownError,
        PortTimeoutError,
        PortUnauthorizedAccessError
    }
}
