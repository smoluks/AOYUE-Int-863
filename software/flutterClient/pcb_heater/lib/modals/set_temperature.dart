import 'package:flutter/material.dart';
import 'package:numberpicker/numberpicker.dart';
import 'package:pcb_heater/helpers/protocol_controller.dart';

class SetTemperatureDialog {
  static Future show(
      BuildContext context, double currentValue, bool channel1, bool channel2) {
    int _currentValue = currentValue.round();
    bool _channel1 = channel1;
    bool _channel2 = channel2;
    StateSetter _setState;

    // show the dialog
    return showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text("Set target temperature"),
          content: StatefulBuilder(
              builder: (BuildContext context, StateSetter setState) {
            _setState = setState;

            return Flex(
                direction: Axis.vertical,
                mainAxisSize: MainAxisSize.min,
                children: [
                  NumberPicker(
                    value: _currentValue,
                    minValue: 0,
                    maxValue: 400,
                    step: 10,
                    haptics: true,
                    itemHeight: 100,
                    axis: Axis.horizontal,
                    onChanged: (value) => _setState(() {
                      _currentValue = value;
                    }),
                    decoration: BoxDecoration(
                      borderRadius: BorderRadius.circular(8),
                      border: Border.all(color: Colors.black26, width: 2.0),
                    ),
                    textStyle: const TextStyle(fontSize: 20),
                    selectedTextStyle: const TextStyle(fontSize: 20),
                  ),
                  Row(children: [
                    Checkbox(
                      value: _channel1,
                      onChanged: (bool? value) {
                        setState(() {
                          _channel1 = value!;
                        });
                      },
                    ),
                    const Text("Channel 1")
                  ]),
                  Row(children: [
                    Checkbox(
                      value: _channel2,
                      onChanged: (bool? value) {
                        setState(() {
                          _channel2 = value!;
                        });
                      },
                    ),
                    const Text("Channel 2")
                  ]),
                ]);
          }),
          actions: [
            TextButton(
              child: const Text("Cancel"),
              onPressed: () {
                Navigator.of(context).pop(null);
              },
            ),
            TextButton(
              child: const Text("OK"),
              onPressed: () {
                Navigator.of(context).pop(Target.constructor2(
                    _currentValue.toDouble(), _channel1, _channel2));
              },
            ),
          ],
        );
      },
    );
  }
}
