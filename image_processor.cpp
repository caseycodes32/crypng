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

void InitializeRandomSeed()
{
    std::srand(std::time(0));
}

void GenerateRandomKey(unsigned char *key, size_t length)
{
    for (int i = 0; i < length; i++)
        key[i] = std::rand() % (0xFF + 1);
}

bool GenerateRandomBit(float prob)
{
    int rand = std::rand() % 100;

    return (rand < (prob * 100));
}

bool GetNthBitFromByte(unsigned char byte, int n)
{
    return (byte >> n) & 1;
}

bool GetBitFromArray(unsigned char *message, size_t index)
{
    size_t byte_idx = index / 8;
    size_t bit_idx = index % 8;

    return GetNthBitFromByte(message[byte_idx], bit_idx);
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
            
            for (int c = 0; c < std::min(image_details.channels, 3); c++)
            {
                image_details.data[idx + c] =  image_details.data[idx + c] << 7;
            }
        }
    }
}

void LSBtoMSBChannel(ImageDetails image_details, int channel)
{
    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int idx = (y * image_details.width + x) * image_details.channels;
            
            for (int c = 0; c < image_details.channels; c++)
            {
                if (c == channel)
                    image_details.data[idx + c] =  image_details.data[idx + c] << 7;
                else
                    image_details.data[idx + c] =  image_details.data[idx + c] >> 8;
            }
        }
    }
}

void PopulateBitArray(bool *second_lsb, ImageDetails image_details)
{
    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int idx = (y * image_details.width + x) * image_details.channels;
            
            for (int c = 0; c < image_details.channels; c++)
            {
                second_lsb[idx + c] = GetNthBitFromByte(image_details.data[idx + c], 1);
            }
        }
    }
}

std::vector<Block> CreateBlockList(bool *bits, ImageDetails image_details)
{
    std::vector<Block> block_list;

    int block_width = BLOCK_SIZE * image_details.channels;
    int block_bytes = block_width * BLOCK_SIZE;
    int intensity_channels = std::min(image_details.channels, 3);

    for (int y = 0; y < image_details.height - BLOCK_SIZE; y += BLOCK_SIZE)
    {
        for (int x = 0; x < image_details.width - BLOCK_SIZE; x += BLOCK_SIZE)
        {
            for (int c = 0; c < intensity_channels; c++)
            {
                Block current_block;
                current_block.loc_x = x;
                current_block.loc_y = y;
                current_block.length = block_bytes;
                current_block.image_channels = image_details.channels;
                current_block.block_channel = c;
                current_block.block_bits = new bool[block_bytes];

                for (int i = 0; i < BLOCK_SIZE; i++)
                    memcpy(current_block.block_bits + (i * block_width), bits + (((y + i) * image_details.width + x) * image_details.channels), block_width);

                CalculateBlockStats(current_block, c);
                block_list.push_back(current_block);
                delete(current_block.block_bits);
            }
        }
    }
    return block_list;
}

void CalculateBlockStats(Block &block, int channel)
{
    int n = BLOCK_SIZE * BLOCK_SIZE;

    double mean;
    double variance;

    for (int i = channel; i < block.length; i += block.image_channels)
        mean += block.block_bits[i];

    mean /= n;
    block.mean = mean;

    for (int i = channel; i < block.length; i += block.image_channels)
        variance += ((block.block_bits[i] - mean) * (block.block_bits[i] - mean));
    
    variance /= (n - 1);
    
    block.var = variance;
}

// Quicksort functions PartitionBlocks() and QuicksortBlocks() derived from https://www.geeksforgeeks.org/cpp-program-for-quicksort/
int PartitionBlocks(std::vector<Block> &vec_blocks, int idx_low, int idx_high)
{
    double pivot = vec_blocks.at(idx_high).var;
    
    int i = (idx_low - 1);

    for (int j = idx_low; j <= idx_high - 1; j++)
    {
        if (vec_blocks.at(j).var < pivot)
        {
            i++;
            std::swap(vec_blocks[i], vec_blocks[j]);
        }
    }

    std::swap(vec_blocks[i + 1], vec_blocks[idx_high]);
    
    return (i + 1);
}

void QuicksortBlocks(std::vector<Block> &vec_blocks, int idx_low, int idx_high)
{
    if (idx_low < idx_high)
    {
        int partition_idx = PartitionBlocks(vec_blocks, idx_low, idx_high);

        QuicksortBlocks(vec_blocks, idx_low, partition_idx - 1);
        QuicksortBlocks(vec_blocks, partition_idx + 1, idx_high);
    }
}

void WriteMessageToHighVarianceBlockLSB(unsigned char *message_buffer, int message_length, std::vector<Block> vec_blocks, ImageDetails image_details)
{
    size_t message_buf_iterator_bit = 0;

    for (int i = vec_blocks.size() - 1; i >= 0; i--)
    {   
        for (int by = 0;  by < BLOCK_SIZE; by++)
        {
            for (int bx = 0;  bx < BLOCK_SIZE; bx++)
            {
                if ((message_buf_iterator_bit / 8) < message_length)
                {
                    int image_byte_loc = (((vec_blocks.at(i).loc_y + by) * image_details.width) + vec_blocks.at(i).loc_x + bx) * image_details.channels;

                    bool bit_mask = GetNthBitFromByte(message_buffer[message_buf_iterator_bit / 8], message_buf_iterator_bit % 8);

                    image_details.data[image_byte_loc + vec_blocks.at(i).block_channel] = image_details.data[image_byte_loc + vec_blocks.at(i).block_channel] >> 1;
                    image_details.data[image_byte_loc + vec_blocks.at(i).block_channel] = image_details.data[image_byte_loc + vec_blocks.at(i).block_channel] << 1;
                    image_details.data[image_byte_loc + vec_blocks.at(i).block_channel] = image_details.data[image_byte_loc + vec_blocks.at(i).block_channel] | bit_mask;

                    message_buf_iterator_bit++;
                }
                else return;
            }
        }
    }
}

void ReadMessageFromHighVarianceBlockLSB(unsigned char *message_buffer, int message_length, std::vector<Block> vec_blocks, ImageDetails image_details)
{
    size_t message_buf_iterator_bit = 0;

    for (int i = vec_blocks.size() - 1; i >= 0; i--)
    {   
        for (int by = 0;  by < BLOCK_SIZE; by++)
        {
            for (int bx = 0;  bx < BLOCK_SIZE; bx++)
            {
                if ((message_buf_iterator_bit / 8) < message_length)
                {
                    int image_byte_loc = (((vec_blocks.at(i).loc_y + by) * image_details.width) + vec_blocks.at(i).loc_x + bx) * image_details.channels;

                    bool current_bit = GetNthBitFromByte(image_details.data[image_byte_loc + vec_blocks.at(i).block_channel], 0);

                    AppendBitToArray(message_buffer, message_buf_iterator_bit, current_bit);

                    message_buf_iterator_bit++;
                }
                else return;
            }
        }
    }
}

std::size_t HashMemory(unsigned char *data, int length)
{
    size_t hash_value = 0;
    for (int i = 0; i < length; i++)
    {
        std::bitset<8> bit_i = data[i];
        std::hash<std::bitset<8>> bitset_hash;
        hash_value = hash_value ^ bitset_hash(bit_i);
    }
    return hash_value;
}

void XCrypt::PerformEncryptionPipeline(char *message, int message_length, unsigned char *private_key, int key_length, ImageDetails image_details, bool &complete)
{
    struct AES_ctx ctx;
    unsigned char init_vector[key_length];
    memset(init_vector, 0x9D, key_length);

    int message_buffer_len = message_length;
    if (message_buffer_len % AES_BLOCKLEN)
        message_buffer_len += AES_BLOCKLEN - (message_buffer_len % AES_BLOCKLEN);

    if (message_buffer_len == AES_BLOCKLEN)
        message_buffer_len = (AES_BLOCKLEN * 2);

    if (message_buffer_len == 0)
        return;

    long long delta_mod_key_hash = 0;
    while (true)
    {
        GenerateRandomKey(private_key, key_length);
        size_t private_key_hash = HashMemory(private_key, key_length);

        delta_mod_key_hash = (((private_key_hash % 4096) * AES_BLOCKLEN) + AES_BLOCKLEN) - message_buffer_len; // 65536 (max message buf) / 4
        if (delta_mod_key_hash == 0) break;
    }

    unsigned char *encrypted_message_buffer = new unsigned char[message_buffer_len];

    memset(encrypted_message_buffer, 0x00, message_buffer_len);
    memcpy(encrypted_message_buffer, message, message_buffer_len);

    AES_init_ctx_iv(&ctx, private_key, init_vector);
    AES_CBC_encrypt_buffer(&ctx, encrypted_message_buffer, message_buffer_len);
    
    int bit_array_len = image_details.width * image_details.height * image_details.channels;
    bool *second_bits = new bool[bit_array_len];

    PopulateBitArray(second_bits, image_details);

    std::vector<Block> second_bit_blocks = CreateBlockList(second_bits, image_details);
    delete(second_bits);

    QuicksortBlocks(second_bit_blocks, 0, second_bit_blocks.size() - 1);

    WriteMessageToHighVarianceBlockLSB(encrypted_message_buffer, message_buffer_len, second_bit_blocks, image_details);
    delete(encrypted_message_buffer);

    complete = true;
}

void XCrypt::ThreadPerformEncryptionPipeline(char *message, int message_length, unsigned char *private_key, int key_length, ImageDetails image_details, bool &complete)
{
    std::thread thread_pep(PerformEncryptionPipeline, message, message_length, private_key, key_length, image_details, std::ref(complete));
    thread_pep.detach();
}

void XCrypt::PerformDecryptionPipeline(char *message_buffer, int &message_length, unsigned char *private_key, int key_length, ImageDetails image_details, bool &complete)
{
    struct AES_ctx ctx;
    unsigned char init_vector[key_length];
    memset(init_vector, 0x9D, key_length);

    size_t private_key_hash = HashMemory(private_key, key_length);
    int decoded_message_length = (((private_key_hash % 4096) * AES_BLOCKLEN) + AES_BLOCKLEN);

    message_length = decoded_message_length;

    unsigned char *decrypted_message_buffer = new unsigned char[decoded_message_length];
    memset(decrypted_message_buffer, 0x00, decoded_message_length);

    int bit_array_len = image_details.width * image_details.height * image_details.channels;
    bool *second_bits = new bool[bit_array_len];

    PopulateBitArray(second_bits, image_details);

    std::vector<Block> second_bit_blocks = CreateBlockList(second_bits, image_details);
    delete(second_bits);

    QuicksortBlocks(second_bit_blocks, 0, second_bit_blocks.size() - 1);

    ReadMessageFromHighVarianceBlockLSB(decrypted_message_buffer, decoded_message_length, second_bit_blocks, image_details);

    AES_init_ctx_iv(&ctx, private_key, init_vector);
    AES_CBC_decrypt_buffer(&ctx, decrypted_message_buffer, decoded_message_length);

    memcpy(message_buffer, decrypted_message_buffer, decoded_message_length);

    delete(decrypted_message_buffer);

    complete = true;
}

void XCrypt::ThreadPerformDecryptionPipeline(char *message_buffer, int &message_length, unsigned char *private_key, int key_length, ImageDetails image_details, bool &complete)
{
    std::thread thread_pdp(PerformDecryptionPipeline, message_buffer, std::ref(message_length), private_key, key_length, image_details, std::ref(complete));
    thread_pdp.detach();
}