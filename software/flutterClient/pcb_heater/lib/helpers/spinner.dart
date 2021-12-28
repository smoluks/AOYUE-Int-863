import 'package:flutter/material.dart';

class Spinner {
  final BuildContext _context;
  bool _isShowed = false;

  Spinner(this._context);

  void show(String text) {
    _isShowed = true;
    showDialog(
      context: _context,
      barrierDismissible: false,
      builder: (BuildContext context) {
        return WillPopScope(
          onWillPop: null,
          child: new AlertDialog(
            contentPadding: EdgeInsets.only(
              top: 20,
              bottom: 20,
              left: 20,
              right: 20,
            ),
            // decoration: new BoxDecoration(
            //   color: Colors.white,
            //   shape: BoxShape.rectangle,
            //   borderRadius: BorderRadius.circular(4),
            //   boxShadow: [
            //     BoxShadow(
            //       color: Colors.black26,
            //       blurRadius: 10.0,
            //       offset: const Offset(0.0, 10.0),
            //     ),
            //   ],
            // ),
            content: new Row(
              children: <Widget>[
                new CircularProgressIndicator(),
                new Container(width: 15, height: 0),
                new Text(text),
              ],
            ),
            actions: <Widget>[],
          ),
        );
      },
    );
  }

  void hide() {
    if (_isShowed) Navigator.of(_context).pop();
  }
}
