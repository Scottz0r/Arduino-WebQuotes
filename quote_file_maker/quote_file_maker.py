import sys

MAX_NAME_LEN = 31
MAX_QUOTE_LEN = 123
MAX_WORD_LEN = 63


def validate_input(name, quote):
    if len(name) > MAX_NAME_LEN:
        raise ValueError(f"Name '{name}' too long.")

    if len(quote) > MAX_QUOTE_LEN:
        raise ValueError(f"Quote '{quote}' too long.")

    for word in quote.split(" "):
        if len(word) > MAX_WORD_LEN:
            raise ValueError(f"Word '{word}' too large.")


def main():
    if len(sys.argv) != 3:
        print("Invalid arguments.")
        exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    print(f"Generating quote binary file from '{input_file}' to '{output_file}'...")

    # List to hold index entries.
    index = []

    # Build the raw data part from the file.
    raw_data = bytearray()
    with open(input_file, "r") as fp:
        for line in fp:
            line = line.strip()
            name, quote = line.split("|")

            validate_input(name, quote)

            # Add the relative index of this item to the index.
            index.append(len(raw_data))

            raw_data.append(len(name))
            raw_data.append(len(quote))

            raw_data.extend(name.encode("ascii"))
            raw_data.extend(quote.encode("ascii"))

    # Make a header for the project. The header will be Number of Items (2 bytes).
    header = bytearray()
    header.append(len(index) & 0xFF)
    header.append(len(index) >> 8)

    # Build an index into the raw data for constant time seeking to a specific quote. Make an exact address by adding
    # the length of the header and the length of this index.
    index_byte_len = len(index) * 2
    raw_index = bytearray()
    for i in range(0, len(index)):
        exact_address = len(header) + index_byte_len + index[i]
        raw_index.append(exact_address & 0xFF)
        raw_index.append(exact_address >> 8)

    # Write all data to the binary file.
    with open(output_file, "wb") as fp:
        fp.write(header)
        fp.write(raw_index)
        fp.write(raw_data)

    print(f"Done!")


if __name__ == "__main__":
    main()
