#pragma once

#define AES128 1
#define CBC 1

#include <random>
#include <ctime>
#include "aes.h"
#include "image_handler.h"

const uint8_t LSB_ZERO = 0b00000000;
const uint8_t LSB_ONE = 0b00000001;
const uint8_t BYTE_VAL[2] = {LSB_ZERO, LSB_ONE};

std::string test_aes();
void InitializeRandomSeed();
void GenerateRandomKey(unsigned char *key, size_t length);
bool GetBitFromArray(unsigned char *message, size_t index);
void EncodeMessageLinear(unsigned char *message, size_t length, int channel, ImageDetails image_details);
std::string DecodeMessageLinear(size_t length, int channel, ImageDetails image_details);

void TestEncode(ImageDetails image_details);
std::string TestDecode(ImageDetails image_details);

void ZeroLSB(ImageDetails image_details);
void LSBtoMSB(ImageDetails image_details);