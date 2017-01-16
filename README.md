# Dart Native Extension with Dynamsoft Barcode Reader

## Code Snippet

```Dart
import 'sample_asynchronous_extension.dart';

void main() {
  BarcodeReader reader = new BarcodeReader();
  reader.readBarcode(r'f:\AllSupportedBarcodeTypes.bmp').then((values) {
    if (values != null) {
      for (var number in values) {
        print(number);
      }
    }
  });
}

```

## Reference
[Native Extensions for the Standalone Dart VM][1]

[1]:https://www.dartlang.org/articles/dart-vm/native-extensions
