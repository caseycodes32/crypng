#include "image_processor.h"

std::string test_aes()
{
    std::string temp;

    unsigned char private_key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    unsigned char init_vector[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    unsigned char text[]  = { 0x68, 0x65, 0x68, 0x65, 0x20, 0x73, 0x65, 0x63, 0x72, 0x65, 0x74, 0x20,
                            0x74, 0x65, 0x78, 0x74, 0x20, 0x6E, 0x79, 0x61, 0x20, 0x3A, 0x33, 0x20,
                            0x6F, 0x6B, 0x20, 0x6D, 0x61, 0x6B, 0x65, 0x20, 0x74, 0x68, 0x69, 0x73,
                            0x20, 0x61, 0x20, 0x6C, 0x69, 0x74, 0x74, 0x6C, 0x65, 0x20, 0x6C, 0x6F,
                            0x6E, 0x67, 0x65, 0x72, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x64, 0x69, 0x76,
                            0x69, 0x73, 0x69, 0x62 };

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, private_key, init_vector);
    AES_CBC_encrypt_buffer(&ctx, text, 64);
    AES_init_ctx_iv(&ctx, private_key, init_vector);
    AES_CBC_decrypt_buffer(&ctx, text, 64);

    return std::string(reinterpret_cast<char*>(text), 64);
}

bool GetBitFromArray(unsigned char *message, size_t index)
{
    size_t byte_idx = index / 8;
    size_t bit_idx = index % 8;

    return (message[byte_idx] >> bit_idx) & 1;
}

void AppendBitToArray(unsigned char *message, size_t index, int bit)
{
    size_t byte_idx = index / 8;
    size_t bit_idx = index % 8;

    unsigned char byte_mask = bit;
    byte_mask = byte_mask << bit_idx;

    message[byte_idx] = message[byte_idx] | byte_mask;
}

void EncodeMessageLinear(unsigned char *message, size_t length, int channel, ImageDetails image_details)
{
    size_t MAX_MESSAGE_BITS = length * 8;
    size_t bit_idx = 0;

    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int image_byte_idx = (y * image_details.width + x) * image_details.channels;
            
            image_details.data[image_byte_idx + channel] =  image_details.data[image_byte_idx + channel] >> 1;
            image_details.data[image_byte_idx + channel] =  image_details.data[image_byte_idx + channel] << 1;

            if (bit_idx < MAX_MESSAGE_BITS)
            {
                image_details.data[image_byte_idx + channel] = (image_details.data[image_byte_idx + channel] | BYTE_VAL[GetBitFromArray(message, bit_idx)]);
                bit_idx += 1;
            }
        }
    }
}

std::string DecodeMessageLinear(size_t length, int channel, ImageDetails image_details)
{
    size_t MAX_MESSAGE_BITS = length * 8;
    size_t bit_idx = 0;
    unsigned char message_buf[length] = "";

    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int image_byte_idx = (y * image_details.width + x) * image_details.channels;

            if (bit_idx < MAX_MESSAGE_BITS)
            {
                AppendBitToArray(message_buf, bit_idx, (image_details.data[image_byte_idx + channel] & 1));
                bit_idx += 1;
            }
        }
    }

    return std::string(reinterpret_cast<char*>(message_buf), 64);
}

void TestEncode(ImageDetails image_details)
{
    unsigned char text[]  = {0x68, 0x65, 0x68, 0x65, 0x20, 0x73, 0x65, 0x63, 0x72, 0x65, 0x74, 0x20,
        0x74, 0x65, 0x78, 0x74, 0x20, 0x6E, 0x79, 0x61, 0x20, 0x3A, 0x33, 0x20,
        0x6F, 0x6B, 0x20, 0x6D, 0x61, 0x6B, 0x65, 0x20, 0x74, 0x68, 0x69, 0x73,
        0x20, 0x61, 0x20, 0x6C, 0x69, 0x74, 0x74, 0x6C, 0x65, 0x20, 0x6C, 0x6F,
        0x6E, 0x67, 0x65, 0x72, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x64, 0x69, 0x76,
        0x69, 0x73, 0x69, 0x62};

    EncodeMessageLinear(text, 64, 0, image_details);
}

std::string TestDecode(ImageDetails image_details)
{
    std::string decoded_message = DecodeMessageLinear(64, 0, image_details);

    return decoded_message;
}

void ZeroLSB(ImageDetails image_details)
{
    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int idx = (y * image_details.width + x) * 4;
            
            image_details.data[idx] =  image_details.data[idx] >> 1;
            image_details.data[idx] =  image_details.data[idx] << 1;
            
            image_details.data[idx + 1] =  image_details.data[idx + 1] >> 1;
            image_details.data[idx + 1] =  image_details.data[idx + 1] << 1;

            image_details.data[idx + 2] =  image_details.data[idx + 2] >> 1;
            image_details.data[idx + 2] =  image_details.data[idx + 2] << 1;
        }
    }
}

void LSBtoMSB(ImageDetails image_details)
{
    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int idx = (y * image_details.width + x) * image_details.channels;
            
            image_details.data[idx] =  image_details.data[idx] >> 1;
            image_details.data[idx] =  image_details.data[idx] << 7;
            
            if (image_details.channels > 1)
            {
                image_details.data[idx + 1] = 0;

                image_details.data[idx + 2] = 0;
            }
        }
    }
}