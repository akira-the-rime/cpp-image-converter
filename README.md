ENG: 
Image Converter.
This program is designed for easy conversion of .ppm, .jpeg and .bmp images.
It requires the C++17 language standard.
Tech stack: C++, C, OOP, STL and CMake.
1) To use the program, determine the "in" and "out" files' formats, using GetFormatInterface() function. The program uses the LibJPEG library, but it has been adapted to the C++ language.
2) Load the contents of the "in" file into an "Image" class object, using LoadImage() method
3) Save the converted image to the "out" file with SaveImage() method

RU:
Конвертер изображений.
Эта программа разработана для простой конвертации изображений форматов .ppm, .jpeg и .bmp.
Для запуска программы требуется стандарт языка C++17.
Стек технологий: C++, C, ООП, STL и CMake.
1) Чтобы использовать программу, определите форматы файлов ввода и вывода с помощью функции GetFormatInterface(). Программа использует библиотеку LibJPEG, адаптированную под C++
2) Загрузите содержимое файла "in" в объект класса "Image" с помощью метода LoadImage().
3) Сохраните преобразованное изображение в файл "out" с помощью метода SaveImage().
