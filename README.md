# Dart Native Extension with Dynamsoft Barcode Reader
Create a Dart asynchronous extension with [Dynamsoft C/C++ barcode SDK][2].

## Code Snippet
A simple dart barcode reader:

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

![dart barcode reader](http://www.codepool.biz/wp-content/uploads/2017/01/dart-console-barcode-reader.PNG)

## Reference
[Native Extensions for the Standalone Dart VM][1]

## Blog
[Building Dart Native Extension with Dynamsoft Barcode SDK on Windows][3]

[1]:https://www.dartlang.org/articles/dart-vm/native-extensions
[2]:http://www.dynamsoft.com/Products/dynamsoft-webcam-sdk.aspx
[3]:http://www.codepool.biz/dart-native-extension-barcode-sdk.html
