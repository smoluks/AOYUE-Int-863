import '/enums/connection_type.dart';

class Device {
  ConnectionType type;
  String address;
  String name;
  Device(this.type, this.name, this.address);
}
