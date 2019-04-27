#include "Instructions.h"
#include "BufferTools.h"

size_t Copy_Instruction::SIZE() const {
	return sizeof(char) + (sizeof(size_t) * 3ull);
}

void Copy_Instruction::DO(Buffer & bufferNew, const Buffer & bufferOld) const {
	for (auto i = index, x = beginRead; i < bufferNew.size() && x < endRead && x < bufferOld.size(); ++i, ++x)
		bufferNew[i] = bufferOld[x];
}

void Copy_Instruction::WRITE(void ** pointer) const {
	// Write Type
	*reinterpret_cast<char*>(*pointer) = TYPE;
	*pointer = BFT::PTR_ADD(*pointer, sizeof(char));
	// Write Index
	std::memcpy(*pointer, &index, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Write Begin
	std::memcpy(*pointer, &beginRead, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Write End
	std::memcpy(*pointer, &endRead, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
}

Copy_Instruction Copy_Instruction::READ(void ** pointer) {
	// Type already read
	Copy_Instruction inst;
	// Read Index
	inst.index = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Read Begin
	inst.beginRead = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Read End
	inst.endRead = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	return inst;
}

size_t Insert_Instruction::SIZE() const {
	return sizeof(char) + (sizeof(size_t) * 2) + (sizeof(char) * newData.size());
}

void Insert_Instruction::DO(Buffer & bufferNew, const Buffer &) const {
	for (auto i = index, x = size_t(0ull), length = newData.size(); i < bufferNew.size() && x < length; ++i, ++x)
		bufferNew[i] = reinterpret_cast<std::byte&>(const_cast<char&>(newData[x]));
}

void Insert_Instruction::WRITE(void ** pointer) const {
	// Write Type
	*reinterpret_cast<char*>(*pointer) = TYPE;
	*pointer = BFT::PTR_ADD(*pointer, sizeof(char));
	// Write Index
	std::memcpy(*pointer, &index, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Write Length
	auto length = newData.size();
	std::memcpy(*pointer, &length, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	if (length) {
		// Write Data
		std::memcpy(*pointer, newData.data(), length);
		*pointer = BFT::PTR_ADD(*pointer, length);
	}
}

Insert_Instruction Insert_Instruction::READ(void ** pointer) {
	// Type already read
	Insert_Instruction inst;
	// Read Index
	inst.index = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Read Length
	size_t length = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	if (length) {
		// Read Data
		inst.newData.resize(length);
		std::memcpy(inst.newData.data(), *pointer, length);
		*pointer = BFT::PTR_ADD(*pointer, length);
	}
	return inst;
}

size_t Repeat_Instruction::SIZE() const {
	return sizeof(char) + (sizeof(size_t) * 2ull) + sizeof(char);
}

void Repeat_Instruction::DO(Buffer & bufferNew, const Buffer &) const {
	for (auto i = index, x = size_t(0ull); i < bufferNew.size() && x < amount; ++i, ++x)
		bufferNew[i] = reinterpret_cast<std::byte&>(const_cast<char&>(value));
}

void Repeat_Instruction::WRITE(void ** pointer) const {
	// Write Type
	*reinterpret_cast<char*>(*pointer) = TYPE;
	*pointer = BFT::PTR_ADD(*pointer, sizeof(char));
	// Write Index
	std::memcpy(*pointer, &index, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Write Amount
	std::memcpy(*pointer, &amount, sizeof(size_t));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Write Value
	std::memcpy(*pointer, &value, sizeof(char));
	*pointer = BFT::PTR_ADD(*pointer, sizeof(char));
}

Repeat_Instruction Repeat_Instruction::READ(void ** pointer) {
	// Type already read
	Repeat_Instruction inst;
	// Read Index
	inst.index = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Read Amount
	inst.amount = *reinterpret_cast<size_t*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(size_t));
	// Read Value
	inst.value = *reinterpret_cast<char*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(char));
	return inst;
}

InstructionTypes Instruction_Maker::Make(void ** pointer) {
	const char type = *reinterpret_cast<char*>(*pointer);
	*pointer = BFT::PTR_ADD(*pointer, sizeof(char));
	switch (type) {
	case Copy_Instruction::TYPE:
		return Copy_Instruction::READ(pointer);
	case Insert_Instruction::TYPE:
		return Insert_Instruction::READ(pointer);
	case Repeat_Instruction::TYPE:
		return Repeat_Instruction::READ(pointer);
	}
	return {};
}