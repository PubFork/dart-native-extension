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
