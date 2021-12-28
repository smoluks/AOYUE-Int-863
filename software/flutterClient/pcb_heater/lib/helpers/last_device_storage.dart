import 'package:shared_preferences/shared_preferences.dart';
import '/dto/device.dart';
import '/enums/connection_type.dart';

class LastDeviceStorage {
  static const typeKey = "last_conn_type";
  static const nameKey = "last_conn_name";
  static const identifierKey = "last_conn_identifier";

  //Load last using device from system storage
  static Future<Device?> tryLoadLastDevice() async {
    final prefs = await SharedPreferences.getInstance();

    try {
      //check address saved
      final value = prefs.getInt(typeKey);
      if (value == null) {
        return null;
      }

      var connType = ConnectionType.values[value];
      switch (connType) {
        case ConnectionType.bt:
          final address = prefs.getString(identifierKey);
          if (address != null) {
            return Device(
              ConnectionType.bt,
              prefs.getString(nameKey) ?? "PCB Heater",
              address,
            );
          }
          break;

        case ConnectionType.com:
          //final comPort = prefs.getString("last_conn_com_port");
          //if (address != null) await useTCPDevice(address);
          break;
      }
    } on RangeError {
      return null;
    }
  }

  static SaveLastDevice(device) {}
}
