import 'package:flutter/widgets.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:pcb_heater/helpers/dialog_helper.dart';
import 'package:permission_handler/permission_handler.dart';

class PermissionHelper {
  static Future<bool> tryGetPermissionForBTScan(BuildContext context) async {
    if (!(await checkBTPermission(context))) return false;
    if (!(await checkBTEnable(context))) return false;
    if (!(await checkGeoPermission(context))) return false;

    return true;
  }

  static Future<bool> tryGetPermissionForBT(BuildContext context) async {
    if (!(await checkBTPermission(context))) return false;
    if (!(await checkBTEnable(context))) return false;

    return true;
  }

  //check BT enable
  static Future<bool> checkBTEnable(BuildContext context) async {
    var state = await FlutterBluetoothSerial.instance.state;
    if (state.isEnabled) return true;

    var result = await FlutterBluetoothSerial.instance.requestEnable();
    if (result == null || result == false) {
      await DialogHelper.showInfo(
          context, "BT disabled", "You need to enable bluetooth for using it");
    }

    state = await FlutterBluetoothSerial.instance.state;
    return state.isEnabled;
  }

  //check permission to use BT
  static Future<bool> checkBTPermission(BuildContext context) async {
    var btIsAvailable = await FlutterBluetoothSerial.instance.isAvailable;
    if (btIsAvailable == true) return true;

    await DialogHelper.showInfo(context, "BT not found or permission denied",
        "You need to grant access to bluetooth for using it");

    btIsAvailable = await FlutterBluetoothSerial.instance.isAvailable;
    return btIsAvailable == true;
  }

  //check permission to use geolocation
  static Future<bool> checkGeoPermission(BuildContext context) async {
    var status = Permission.location;
    if (await status.isGranted) return true;

    await status.request();
    if (await status.isGranted) return true;

    await DialogHelper.showInfo(context, "Location permission denied",
        "You need to grant access to location for scan bluetooth devices");

    return false;
  }
}
