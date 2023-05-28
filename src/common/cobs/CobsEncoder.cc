/* internal */
#include "common/cobs/CobsEncoder.h"

namespace Project81
{

uint8_t CobsEncoder::handle_next_zero(const uint8_t *message,
                                      uint8_t &message_index,
                                      std::size_t length,
                                      cobs_chunk_array &output)
{
    if (length == 0)
    {
        return 0;
    }

    uint8_t zero_pointer = 1;

    /* Data will start at the second byte (if there is any data). */
    uint8_t output_index = 1;
    message_index = 0;

    bool zero_found = false;

    while (length and not zero_found and zero_pointer != max_cobs_chunk)
    {
        uint8_t message_curr = message[message_index++];
        if (message_curr == 0)
        {
            zero_found = true;
        }
        else
        {
            output[output_index++] = message_curr;
            zero_pointer++;
        }

        length--;
    }

    output[0] = zero_pointer;
    return output_index;
}

bool CobsEncoder::encode(PcBufferWriter<> &writer)
{
    bool can_write = true;

    while (length and can_write)
    {
        /* Refill the staging buffer if necessary. */
        if (to_write == 0)
        {
            uint8_t message_index;

            to_write =
                handle_next_zero(message, message_index, length, output);

            /*
             * Advance the message forward (for a possible next iteration).
             */
            message += message_index;
            length -= message_index;
        }

        /*
         * Write to the output stream until we run out of bytes to write or
         * it stops accepting new data.
         */
        std::size_t written;
        do
        {
            written = writer.try_push_n(output.data(), to_write);
            to_write -= written;
        } while (to_write and written);

        /*
         * We can't write any more bytes (but we're not done), so avoid
         * busy looping.
         */
        if (written == 0 and to_write)
        {
            can_write = false;
        }
    }

    /*
     * Encoding is complete when there's no message left and no more bytes
     * to write.
     */
    return done();
}

}; // namespace Project81
