import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import '/dto/device.dart';
import '/enums/connection_type.dart';
import '/helpers/dialog_helper.dart';
import '/helpers/last_device_storage.dart';
import '/helpers/permission_helper.dart';
import 'bluetooth_discovery_page.dart';

//import 'BluetoothDiscoveryPage.dart';
//import 'Dto/Device.dart';
//import 'Helpers/DialogHelper.dart';
//import 'SensorsPage.dart';

class SelectConnTypePage extends StatefulWidget {
  const SelectConnTypePage({Key? key}) : super(key: key);

  @override
  _SelectConnTypePage createState() => _SelectConnTypePage();
}

class _SelectConnTypePage extends State<SelectConnTypePage> {
  String? _error;
  bool _loading = true;

  @override
  void initState() {
    super.initState();

    loadLastDevice().whenComplete(() => {
          setState(() {
            _loading = false;
          })
        });
  }

  // Trying to connect to last device
  Future<void> loadLastDevice() async {
    var device = await LastDeviceStorage.tryLoadLastDevice();

    if (device == null) return;

    useDevice(device);
  }

  //Connect over BT button click handler
  Future<void> searchBTDevice() async {
    if (!(await PermissionHelper.tryGetPermissionForBTScan(context))) return;

    //search device
    var device =
        await Navigator.of(context).push(MaterialPageRoute(builder: (context) {
      return const BluetoothDiscoveryPage();
    }));

    //connect
    if (device != null) {
      await LastDeviceStorage.SaveLastDevice(device);
      useDevice(device);
    }
  }

  Future<void> useDevice(Device device) async {
    if (device.type == ConnectionType.bt &&
        !(await PermissionHelper.tryGetPermissionForBT(context))) return;

    /*bool success =
        await Navigator.of(context).push(MaterialPageRoute(builder: (context) {
      //return SensorsPage(device: device);
    }));

    if (success != null && !success) {
      prefs.setInt("last_conn_type", -1);
    }*/
  }

  @override
  Widget build(BuildContext context) {
    if (_error != null) return _getErrorWidget(context);
    if (_loading) return _getLoadingWidget(context);

    return _getMainWidget(context);
  }

  Widget _getMainWidget(BuildContext context) {
    var buttonStyle = ElevatedButton.styleFrom(
        primary: Colors.blue,
        padding: const EdgeInsets.all(8.0),
        minimumSize: const Size(300.0, 40.0),
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)));

    return Scaffold(
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: <Widget>[
          /*Row(mainAxisAlignment: MainAxisAlignment.center, children: <Widget>[
            Text(
              "Cоюзтехнология",
              style: TextStyle(
                fontSize: 30.0,
              ),
            )
          ]),
          Image.asset('assets/logo.png'),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: <Widget>[
            Text(
              "Программа сбора и обработки данных",
              style: TextStyle(
                fontSize: 18.0,
              ),
            )
          ]),
          SizedBox(height: 20),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: <Widget>[
            Text(
              "Мультисенсор v2.0",
              style: TextStyle(fontSize: 24.0, fontWeight: FontWeight.bold),
            )
          ]),*/
          const SizedBox(height: 25),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: <Widget>[
            ElevatedButton(
              style: buttonStyle,
              onPressed: () {
                searchBTDevice();
              },
              child: const Text(
                "Connect over BT",
                style: TextStyle(
                  fontSize: 20.0,
                ),
              ),
            )
          ]),
          const SizedBox(height: 10),
          Row(mainAxisAlignment: MainAxisAlignment.center, children: <Widget>[
            ElevatedButton(
              style: buttonStyle,
              onPressed: () {
                DialogHelper.showInfo(context, "Error", "Not supported yet");
              },
              child: const Text(
                "Connect over COM port",
                style: TextStyle(
                  fontSize: 20.0,
                ),
              ),
            )
          ]),
        ],
      ),
    );
  }

  Widget _getLoadingWidget(BuildContext context) {
    return const Scaffold(body: Center(child: Text('Loading')));
  }

  Widget _getErrorWidget(BuildContext context) {
    return Scaffold(body: Center(child: Text(_error!)));
  }
}
