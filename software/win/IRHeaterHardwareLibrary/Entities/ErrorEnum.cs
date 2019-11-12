namespace IRHeaterHardwareLibrary.Data
{
    public enum ErrorEnum
    {
        NoError,
        NoPorts,
        PortNotFound,
        NoSlave,
        PortIoError,
        UnknownError,
        PortTimeoutError,
        PortUnauthorizedAccessError
    }
}
