import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:pcb_heater/dto/device.dart';
import 'package:pcb_heater/enums/connection_type.dart';
import 'package:pcb_heater/helpers/Spinner.dart';

class BluetoothDeviceListEntry {
  BluetoothDevice device;
  int? rssi;
  BluetoothDeviceListEntry(this.device, this.rssi);
}

class BluetoothDiscoveryPage extends StatefulWidget {
  const BluetoothDiscoveryPage({Key? key}) : super(key: key);

  @override
  _BluetoothDiscoveryPage createState() => _BluetoothDiscoveryPage();
}

class _BluetoothDiscoveryPage extends State<BluetoothDiscoveryPage> {
  late StreamSubscription<BluetoothDiscoveryResult> _streamSubscription;
  List<BluetoothDeviceListEntry> results = <BluetoothDeviceListEntry>[];
  bool _isDiscovering = false;
  bool _isBonding = false;

  @override
  void initState() {
    super.initState();

    //add all bonded
    FlutterBluetoothSerial.instance
        .getBondedDevices()
        .then((List<BluetoothDevice> bondedDevices) {
      for (var device in bondedDevices) {
        _addDevice(device, null);
      }
    });

    _startDiscovery();
  }

  @override
  void dispose() {
    // Avoid memory leak (`setState` after dispose) and cancel discovery
    _streamSubscription.cancel();
    FlutterBluetoothSerial.instance.setPairingRequestHandler(null);

    super.dispose();
  }

  void _startDiscovery() {
    setState(() {
      _isDiscovering = true;
    });

    try {
      _streamSubscription =
          FlutterBluetoothSerial.instance.startDiscovery().listen((result) {
        _addDevice(result.device, result.rssi);
      });
    } catch (ex) {
      print('startDiscovery failed: $ex');
      Navigator.of(context).pop(null);
    }

    _streamSubscription.onDone(() {
      setState(() {
        _isDiscovering = false;
      });
    });
  }

  void _addDevice(BluetoothDevice device, int? rssi) {
    setState(() {
      var entries =
          results.where((entry) => entry.device.address == device.address);

      if (entries.isNotEmpty) {
        for (var entry in entries) {
          entry.rssi = rssi;
        }
      } else {
        results.add(BluetoothDeviceListEntry(device, rssi));
      }
    });
  }

  onTap(BluetoothDeviceListEntry f) async {
    if (f.device.isBonded) {
      Navigator.of(context).pop(Device(
          ConnectionType.bt, f.device.name ?? "Noname", f.device.address));
      return;
    }

    if (_isBonding) {
      return;
    }

    var spinner = Spinner(context);
    spinner.show("Bonding...");
    _isBonding = true;

    bool? bonded = false;
    try {
      bonded = await FlutterBluetoothSerial.instance
          .bondDeviceAtAddress(f.device.address);
    } catch (e) {
      print("Error occured while bonding : ${e.toString()}");
    } finally {
      spinner.hide();
      _isBonding = false;
    }

    if (bonded == true) {
      Navigator.of(context).pop(Device(
          ConnectionType.bt, f.device.name ?? "Noname", f.device.address));
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Select device'),
        actions: <Widget>[
          _isDiscovering
              ? FittedBox(
                  child: Container(
                    margin: const EdgeInsets.all(16.0),
                    child: const CircularProgressIndicator(
                      valueColor: AlwaysStoppedAnimation<Color>(
                        Colors.white,
                      ),
                    ),
                  ),
                )
              : IconButton(
                  icon: const Icon(Icons.replay),
                  onPressed: _startDiscovery,
                )
        ],
      ),
      body: ListView(children: results.map((f) => _getView(f)).toList()),
    );
  }

  ListTile _getView(BluetoothDeviceListEntry f) {
    return ListTile(
        onTap: () async {
          await onTap(f);
        },
        //onLongPress: onLongPress,
        //enabled: enabled,
        leading: const Icon(
            Icons.devices), // @TODO . !BluetoothClass! class aware icon
        title: Text(f.device.name ?? "Unknown device"),
        subtitle: Text(f.device.address.toString()),
        trailing: Row(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            f.device.isConnected
                ? const Icon(Icons.import_export)
                : const SizedBox(width: 0, height: 0),
            f.device.isBonded
                ? const Icon(Icons.link)
                : const SizedBox(width: 0, height: 0),
            f.rssi != null
                ? Container(
                    margin: const EdgeInsets.all(8.0),
                    child: DefaultTextStyle(
                      style: _computeTextStyle(f.rssi!),
                      child: Column(
                        mainAxisSize: MainAxisSize.min,
                        children: <Widget>[
                          Text(f.rssi.toString()),
                          const Text('dBm'),
                        ],
                      ),
                    ),
                  )
                : const SizedBox(width: 0, height: 0),
          ],
        ));
  }

  static TextStyle _computeTextStyle(int rssi) {
    /**/ if (rssi >= -35) {
      return TextStyle(color: Colors.greenAccent[700]);
    } else if (rssi >= -45) {
      return TextStyle(
          color: Color.lerp(
              Colors.greenAccent[700], Colors.lightGreen, -(rssi + 35) / 10));
    } else if (rssi >= -55) {
      return TextStyle(
          color: Color.lerp(
              Colors.lightGreen, Colors.lime[600], -(rssi + 45) / 10));
    } else if (rssi >= -65) {
      return TextStyle(
          color: Color.lerp(Colors.lime[600], Colors.amber, -(rssi + 55) / 10));
    } else if (rssi >= -75) {
      return TextStyle(
          color: Color.lerp(
              Colors.amber, Colors.deepOrangeAccent, -(rssi + 65) / 10));
    } else if (rssi >= -85) {
      return TextStyle(
          color: Color.lerp(
              Colors.deepOrangeAccent, Colors.redAccent, -(rssi + 75) / 10));
    } else {
      return const TextStyle(color: Colors.redAccent);
    }
  }
}
