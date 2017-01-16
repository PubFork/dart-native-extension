library sample_asynchronous_extension;

import 'dart:async';
import 'dart:isolate';
import 'dart-ext:sample_extension';

class BarcodeReader {
  static SendPort _port;

  Future<List> readBarcode(String filename) {
    var completer = new Completer();
    var replyPort = new RawReceivePort();
    var args = new List(2);
    args[0] = filename;
    args[1] = replyPort.sendPort;
    _servicePort.send(args);
    replyPort.handler = (result) {
      replyPort.close();
      if (result != null) {
        completer.complete(result);
      } else {
        completer.completeError(new Exception("Reading barcode failed"));
      }
    };
    return completer.future;
  }

  SendPort get _servicePort {
    if (_port == null) {
      _port = _newServicePort();
    }
    return _port;
  }

  SendPort _newServicePort() native "BarcodeReader_ServicePort";
}
