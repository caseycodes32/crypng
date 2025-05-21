![logo384x120](https://github.com/user-attachments/assets/0857e474-f7b2-4a35-8596-9adf4a05dcf1)
# crypng

crypng is a LSB steganography software that encodes/decodes hidden messages in the pixel data of PNG images. It uses a unique approach that improves secrecy by only encoding the message in noisy parts of the image. In most images, the use of crypng is undetectable, given that the size of the message is appropriate.

## Methodology

crypng first encrypts a given message using AES-CBC. The message length is calculated using a hash of the key, so iteration is used to find an appropriate 128-bit AES key. Next, it performs bit plane slicing to acquire the second bit plane for each intensity channel. Then, it uses a specialized algorithm to score the noise of each of these bit planes. The bit planes are broken up into 8x8 blocks, then quicksort is used to sort them by noise score. Finally, the encoded message is written to the LSBs correlated with the noisiest blocks. 

## Getting Started

crypng can be compiled simply by using mingw-w64.

Steps:
1. Install [mingw-w64](https://www.mingw-w64.org/)
2. Run the build script **build_mingw_release.bat**
3. Run the crypng executable at /Release/crypng.exe

## Usage

To use crypng, first select a PNG image. Then, press "Encode Message," and provide your secret message. Press "Encode." Copy the key phrase provided to you as this is the key to decoding. Finally, press "Save Image."
To decode a message, enter an image and press "Decode Message." Enter your secret key, then press "Confirm Keyphrase" then "Decode Message." Your hidden message will be displayed. 

## Screenshots
![image](https://github.com/user-attachments/assets/8a1fce08-c46b-460e-8a0c-3a5ecf9eaae0)
![image](https://github.com/user-attachments/assets/948a1e10-124a-4460-8cf9-5db032d397f3)




## Compatibility

* Windows 7
* Windows 8
* Windows 8.1
* Windows 10
* Windows 11
