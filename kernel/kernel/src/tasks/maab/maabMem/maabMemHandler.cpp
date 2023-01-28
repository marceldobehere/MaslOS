#include "maabMemHandler.h"
#include "../maabTask.h"

namespace MAAB_MEM
{
	void MbSegHdr::CombineForward()
	{
		if (next == NULL)
			return;

		if (!next->free)
			return;

		if (next == ((MbMemHandler*)memHandler)->lastHdr)
			((MbMemHandler*)memHandler)->lastHdr = this;


		if (next->next != NULL)
			next->next->last = this;


		length = length + next->length + sizeof(MbSegHdr);
		next = next->next;
	}

	void MbSegHdr::CombineBackward()
	{
		if (last != NULL)
			if (last->free)
			{
				last->CombineForward();
			}
	}

	MbSegHdr* MbSegHdr::Split(size_t splitLength)
	{
		if (splitLength < 0x10)
			return NULL;

		int64_t splitSegLength = (length - splitLength) - sizeof(MbSegHdr);
		if (splitSegLength < 0x10)
			return NULL;


		MbSegHdr* newSplitHdr = (MbSegHdr*)((uint64_t)this + splitLength + sizeof(MbSegHdr));
		*newSplitHdr = MbSegHdr();

		if (next != NULL)
			next->last = newSplitHdr;

		newSplitHdr->next = next;
		next = newSplitHdr;
		newSplitHdr->last = this;
		newSplitHdr->length = splitSegLength;
		newSplitHdr->free = free;
		newSplitHdr->magicNum = MbMagicNum;
		newSplitHdr->memHandler = memHandler;
		length = splitLength;

		if (((MbMemHandler*)memHandler)->lastHdr == this)
			((MbMemHandler*)memHandler)->lastHdr = newSplitHdr;


		return newSplitHdr;
	}




	//((MbMemHandler*)memHandler)->


	MbMemHandler::MbMemHandler(void* bufAddr, uint64_t size, uint64_t _trueOffset)
	{
		this->buffer = bufAddr;
		this->bufSize = size;
		this->trueOffset = _trueOffset;
		void* pos = buffer;

		for (size_t i = 0; i < size; i++)
		{
			*((uint8_t*)((uint64_t)buffer+i)) = 0;
		}

		size_t heapLength = size;

		//start = buffer;
		//heapEnd = (void*)((size_t)heapStart + heapLength);
		MbSegHdr* startSeg = (MbSegHdr*)buffer;
		startSeg->length = heapLength - sizeof(MbSegHdr);
		startSeg->next = NULL;
		startSeg->last = NULL;
		startSeg->free = true;
		startSeg->memHandler = (void*)this;
		startSeg->magicNum = MbMagicNum;
		lastHdr = startSeg;
	}



	void* MbMemHandler::MallocMem(size_t size)
	{
		if (size == 0)
			size = 0x10;

		if (size % 0x10 > 0)
		{
			size -= (size % 0x10);
			size += 0x10;
		}
		MbSegHdr* current = (MbSegHdr*)buffer;
		while (true)
		{

			if ((uint64_t)current < 10000)
				return NULL;

			if (current->magicNum != MbMagicNum)
				return NULL;

			if (current->free)
			{
				if (current->length > size)
				{
					if (current->Split(size) == NULL)
					{
						if (current->next == NULL)
							break;

						current = current->next;
						continue;
					}
					current->free = false;
					current->memHandler = (void*)this;
					return (void*)((uint64_t)current + sizeof(MbSegHdr) - (uint64_t)buffer + trueOffset);
				}
				if (current->length == size)
				{
					current->free = false;
					current->memHandler = (void*)this;
					return (void*)((uint64_t)current + sizeof(MbSegHdr) - (uint64_t)buffer + trueOffset);
				}
			}

			if (current->next == NULL)
				break;
			current = current->next;
		}


		return NULL;
	}





	bool MbMemHandler::FreeMem(void* address)
	{
		address = (void*)((uint64_t)address + (uint64_t)buffer - trueOffset);
		if ((uint64_t)address < (uint64_t)buffer || (uint64_t)address >= (uint64_t)buffer + bufSize)
			return false;

		MbSegHdr* segment = ((MbSegHdr*)address) - 1;

		if (segment->magicNum == MbMagicNum)
		{
			if (!segment->free)
			{
				segment->free = true;
				segment->CombineForward();
				segment->CombineBackward();
				return true;
			}
			else
			{
				//Panic("Tried to free already free Segment!");
				return false;
			}
		}
		else
		{
			//Panic("Tried to free invalid Segment!");
			return false;
		}
		return false;
	}


}