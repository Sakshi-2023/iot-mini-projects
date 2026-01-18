# LiteRT model

Dataset: (github.com/Hrushikraj/Data_Analysis_on_Weather_Dataset)[https://raw.githubusercontent.com/Hrushikraj/Data_Analysis_on_Weather_Dataset/refs/heads/main/1.%20Weather%20Data.csv]

1. Use `xxd -i model.tflite > model_data.cc` to generate `model_data.cc`
2. Change `unsigned char` to `const unsigned char` in `model_data.cc` for better performance.
