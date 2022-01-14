import 'dart:async';
import 'dart:math';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:charts_flutter/flutter.dart' as charts;
import 'package:pcb_heater/enums/modes.dart';
import 'package:pcb_heater/enums/sensor_errors.dart';
import 'package:pcb_heater/modals/set_temperature.dart';
import '../dto/sensor.dart';
import '/dto/device.dart';
import '/helpers/protocol_controller.dart';
import '/modals/dialog_helper.dart';

class DevicePage extends StatefulWidget {
  final Device device;

  const DevicePage(this.device, {Key? key}) : super(key: key);

  @override
  _DevicePage createState() => _DevicePage();
}

class _DevicePage extends State<DevicePage> {
  ProtocolController? _protocolController;
  final _channels = <Sensor>[];
  Mode? _currentMode;
  bool _loading = true;
  Timer? _timer;

  @override
  void initState() {
    super.initState();

    _protocolController = ProtocolController(widget.device);
    connect();
  }

  Future<void> connect() async {
    try {
      await _protocolController!.connect();
      var state = await _protocolController!.getState();

      setState(() {
        for (int i = 0; i < state.sensorStates.length; i++) {
          _channels.add(Sensor(i));
        }
      });

      showState(state);

      var targets = await _protocolController!.getTargets();
      setState(() {
        for (int i = 0; i < targets.length; i++) {
          _channels[i].targetTemperature = targets[i].temperature;
          _channels[i].channel1 = targets[i].channel1;
          _channels[i].channel2 = targets[i].channel2;
        }
      });

      int mode = await _protocolController!.getMode();
      setState(() {
        _currentMode =
            Mode.values.where((element) => element.id == mode).single;
      });

      _timer = Timer.periodic(const Duration(seconds: 1), (_) => updateState());

      setState(() {
        _loading = false;
      });
    } on PlatformException catch (e) {
      await DialogHelper.showInfo(
          context, "Connection error", e.message ?? "unknown");

      _timer?.cancel();
      Navigator.of(context).pop(false);
    }
  }

  @override
  void dispose() {
    _protocolController?.dispose();

    _timer?.cancel();

    super.dispose();
  }

  Future<void> updateState() async {
    var state = await _protocolController!.getState();

    showState(state);
  }

  void showState(DeviceState state) {
    setState(() {
      for (int i = 0; i < state.sensorStates.length; i++) {
        _channels[i].setState(state.sensorStates[i]);
      }
    });
  }

  Future<void> onTemperatureClick(Sensor sensor) async {
    Target? dialogResult = await SetTemperatureDialog.show(
        context, sensor.targetTemperature, sensor.channel1, sensor.channel2);

    if (dialogResult == null) return;

    await _protocolController!.setTarget(sensor.sensorNumber, dialogResult);

    setState(() {
      sensor.targetTemperature = dialogResult.temperature;
      sensor.channel1 = dialogResult.channel1;
      sensor.channel2 = dialogResult.channel2;
    });
  }

  onShowChartClick() {
    SystemChrome.setPreferredOrientations([DeviceOrientation.landscapeLeft]);
  }

  onEditClick() {
    SystemChrome.setPreferredOrientations([DeviceOrientation.portraitUp]);
  }

  Future<void> onModeChange(Mode? newValue) async {
    await _protocolController!.setMode(newValue!.id);
  }

  @override
  Widget build(BuildContext context) {
    return OrientationBuilder(builder: (context, orientation) {
      return orientation == Orientation.portrait
          ? _getVerticalView(context)
          : _getHorisontalView(context);
    });
  }

  Widget _getVerticalView(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text(widget.device.name),
          actions: <Widget>[
            _loading
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
                : const Icon(Icons.bluetooth_connected),
            IconButton(
              icon: const Icon(Icons.insert_chart),
              onPressed: () {
                onShowChartClick();
              },
            )
          ],
        ),
        body: ListView(
            children: _loading == true
                ? []
                : ListTile.divideTiles(context: context, tiles: <Widget>[
                    ListTile(
                        title: SizedBox(
                            height: 60,
                            child: DropdownButton<Mode>(
                              value: _currentMode,
                              icon: const Icon(Icons.arrow_downward),
                              elevation: 16,
                              style: const TextStyle(
                                  fontSize: 50, color: Colors.black),
                              underline: Container(
                                height: 0,
                                color: Colors.deepPurpleAccent,
                              ),
                              onChanged: (Mode? newValue) {
                                onModeChange(newValue);

                                setState(() {
                                  _currentMode = newValue!;
                                });
                              },
                              items: Mode.values
                                  .map<DropdownMenuItem<Mode>>((Mode value) {
                                return DropdownMenuItem<Mode>(
                                  value: value,
                                  child: Text(value.name),
                                );
                              }).toList(),
                            )),
                        subtitle:
                            const Text("Mode", style: TextStyle(fontSize: 20))),
                    ..._channels.map((f) => _getSensorsView(f))
                  ]).toList()));
  }

  Widget _getHorisontalView(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text(widget.device.name),
          actions: <Widget>[
            _loading
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
                : const Icon(Icons.bluetooth_connected),
            IconButton(
              icon: const Icon(Icons.edit),
              onPressed: () {
                onEditClick();
              },
            )
          ],
        ),
        body: charts.TimeSeriesChart(
          _channels.map((e) => e.seriesList).toList(),
          animate: false,
          dateTimeFactory: const charts.LocalDateTimeFactory(),
          primaryMeasureAxis: charts.NumericAxisSpec(
              tickProviderSpec:
                  new charts.BasicNumericTickProviderSpec(desiredTickCount: 10),
              viewport: charts.NumericExtents(
                  0, _channels.map((e) => e.targetTemperature).reduce(max))),
          //primaryMeasureAxis: const charts.NumericAxisSpec(
          //    renderSpec: charts.GridlineRendererSpec(
          //        // Tick and Label styling here.
          //        labelStyle: charts.TextStyleSpec(
          //            fontSize: 18, // size in Pts.
          //            color: charts.MaterialPalette.black),
          //        // Change the line colors to match text color.
          //        lineStyle: charts.LineStyleSpec(
          //            color: charts.MaterialPalette.black))),
        ));
  }

  Widget _getSensorsView(Sensor channel) {
    if (channel.error > 0) {
      return ListTile(
          title: SizedBox(
              height: 50,
              child: Text(SensorErrors.values[channel.error].name,
                  style: const TextStyle(fontSize: 30))),
          subtitle: Text(
              "Sensor ${String.fromCharCode(channel.sensorNumber + 65)}",
              style: const TextStyle(fontSize: 20)));
    }

    if (channel.isPresent == false) {
      return ListTile(
          title: const SizedBox(
              height: 50,
              child: Text("Not configured", style: TextStyle(fontSize: 30))),
          subtitle: Text(
              "Sensor ${String.fromCharCode(channel.sensorNumber + 65)}",
              style: const TextStyle(fontSize: 20)));
    }

    return ListTile(
        subtitle: Text(
            "Sensor ${String.fromCharCode(channel.sensorNumber + 65)}",
            style: const TextStyle(fontSize: 20)),
        title: GestureDetector(
            onTap: () {
              onTemperatureClick(channel);
            },
            child: Row(children: [
              Expanded(
                  flex: 6,
                  child: Text(
                      "${channel.currentTemperature.round()}→${channel.targetTemperature.round()}°",
                      style: const TextStyle(fontSize: 50))),
              Column(
                children: [
                  Text(channel.channel1 ? "CH1" : "",
                      style: const TextStyle(fontSize: 20)),
                  Text(channel.channel1 ? "CH2" : "",
                      style: const TextStyle(fontSize: 20))
                ],
              )
            ])));
  }
}
