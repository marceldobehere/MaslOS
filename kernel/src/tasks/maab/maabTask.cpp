#include "maabTask.h"
//#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"
//#include "../../interrupts/interrupts.h"
#include "../../cmdParsing/cstrTools.h"
//#include "../../OSDATA/MStack/MStackM.h"
//#include "terminalInstance.h"
#include "../../kernelStuff/stuff/cstr.h"



TaskMAAB::TaskMAAB(uint32_t codeLen, uint8_t* code, Window* window, TerminalInstance* term)
{
	this->window = window;
	dTerm = term;
	newTerm = (NewTerminalInstance*)(term->newTermInstance);
	programEnded = false;
	done = false;

	//this->code = (uint8_t*)malloc(codeLen);
	//for (int i = 0; i < codeLen; i++)
	//	this->code[i] = code[i];
	//this->codeLen = codeLen;


	memLen = 2000000;
	this->mem = (uint8_t*)malloc(memLen);
	for (int i = 0; i < memLen; i++)
		mem[i] = 0;

	//newTerm->Println("Data:");
	for (int i = 0; i < codeLen; i++)
	{
		mem[i] = code[i];
		//newTerm->Print("{} ", to_string(mem[i]), defCol);
	}
	//newTerm->Println();

	//newTerm->Println("MEM 1: {}", to_string((uint64_t)mem), defCol);

	memHandler = (MAAB_MEM::MbMemHandler*)malloc(sizeof(MAAB_MEM::MbMemHandler));
	*memHandler = MAAB_MEM::MbMemHandler((void*)((uint64_t)mem + codeLen), memLen - codeLen, codeLen);

	//newTerm->Println("MEM 2: {}", to_string((uint64_t)memHandler->buffer), defCol);


	memUserInputLen = 0;
	for (int i = 0; i < 500; i++)
		memUserInput[i] = (char)0;



	waitInput = false;
	gotInput = false;
	programEnded = false;
	type = TaskType::MAAB;


	instrPointer = 0;
	errMsg = "<NO ERRORS>";
	errCode = 0;


	((TerminalInstance*)window->instance)->userlen = 0;
	((TerminalInstance*)window->instance)->takeInput = false;
	waitInput = false;
	writeInputInto = (uint64_t)0;
	//newTerm->Println("<TASK STARTED!>");
	defCol = Colors.white;

	subDeepness = 0;
	for (int i = 0; i < maxSubDeepness; i++)
		subLastPos[i] = 0;
}

void TaskMAAB::Do()
{
	if (waitInput)
	{
		if (!gotInput)
			return;
		waitInput = false;
		gotInput = false;

		void* toWrite = memHandler->MallocMem(memUserInputLen + 1);
		if (toWrite == NULL)
		{
			programEnded = true;
			errCode = 2;
			errMsg = "MALLOC FOR USER INPUT FAILED!";
			return;
		}

		for (int i = 0; i < memUserInputLen; i++)
			*((char*)((uint64_t)mem + (uint64_t)toWrite + i)) = memUserInput[i];
		*((char*)((uint64_t)mem + (uint64_t)toWrite + memUserInputLen)) = 0;

		*((uint64_t*)((uint64_t)mem + writeInputInto)) = (uint64_t)toWrite;

		memUserInputLen = 0;
		for (int i = 0; i < 500; i++)
			memUserInput[i] = (char)0;
	}

	if (instrPointer >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "INSTRUCTION POINTER OUT OF BOUNDS!";
		return;
	}

	for (int sI = 0; sI < cyclesPerCall; sI++)
	{
		if (waitInput)
			return;
		if (done)
			return;
		if (programEnded)
		{
			newTerm->Println();
			newTerm->Println();
			if (errCode == 0)
			{
				newTerm->Println("Program exited with no errors!");
			}
			else
			{
				newTerm->Println("Program exited with error code: {} !", to_string(errCode));
				newTerm->Println("Error Message:");
				newTerm->Println(errMsg);
			}

			done = true;
			return;
		}
		if (errCode != 0)
		{
			programEnded = true;
			return;
		}

		uint8_t instr = mem[instrPointer];

		if (instr == 0) //nop
		{
			instrPointer++;
		}
		else if (instr == 1) //exit
		{
			programEnded = true;
			errCode = 0;
			return;
		}
		else if (instr == 2) //set fix mem to fix val
		{
			if (instrPointer + 20 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint8_t copySize = mem[instrPointer + 1];
			uint64_t copyAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 2));

			if (copyAddr + copySize >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "ADDR OUT OF BOUNDS!";
				return;
			}

			//newTerm->Print("\n<COPY {} BYTES TO ADDR ", to_string(copySize), defCol);
			//newTerm->Println("{}>", to_string(copyAddr), defCol);

			//newTerm->Print("DATA ADDR TO: ");
			//ShowBytes(8, instrPointer + 2);
			//newTerm->Println();
			//newTerm->Print("DATA ADDR TO: ");
			//ShowBytes(8, copyAddr);
			//newTerm->Println();
			//newTerm->Print("DATA: ");
			//ShowBytes(copySize, instrPointer + 10);
			//newTerm->Println();

			for (int i = 0; i < copySize; i++)
				mem[copyAddr + i] = mem[instrPointer + 10 + i];




			instrPointer += 10 + copySize;
		}
		else if (instr == 3) //copy fix mem to fix mem
		{
			if (instrPointer + 17 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint8_t copySize = mem[instrPointer + 1];
			uint64_t copyAddr1 = *((uint64_t*)((uint64_t)mem + instrPointer + 2));
			uint64_t copyAddr2 = *((uint64_t*)((uint64_t)mem + instrPointer + 10));

			if (copyAddr1 + copySize >= memLen || copyAddr2 + copySize >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "ADDR OUT OF BOUNDS!";
				return;
			}


			for (int i = 0; i < copySize; i++)
				mem[copyAddr2 + i] = mem[copyAddr1 + i];

			instrPointer += 18;
		}

		else if (instr == 16) // fix cast
		{
			if (instrPointer + 30 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}

			DatatypeNumber dataType1 = (DatatypeNumber)mem[instrPointer + 1];
			DatatypeNumber dataType2 = (DatatypeNumber)mem[instrPointer + 2];
			uint8_t size1 = datatypeSizes[(uint8_t)dataType1];

			uint64_t addr1 = instrPointer + 3;
			uint64_t addr2 = *((uint64_t*)((uint64_t)mem + instrPointer + 3 + size1));

			Cast(dataType1, addr1, dataType2, addr2);

			instrPointer += 11 + size1;
		}
		else if (instr == 15) // var cast
		{
			if (instrPointer + 30 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}

			DatatypeNumber dataType1 = (DatatypeNumber)mem[instrPointer + 1];
			DatatypeNumber dataType2 = (DatatypeNumber)mem[instrPointer + 2];

			uint64_t addr1 = *((uint64_t*)((uint64_t)mem + instrPointer + 3));
			uint64_t addr2 = *((uint64_t*)((uint64_t)mem + instrPointer + 11));

			Cast(dataType1, addr1, dataType2, addr2);

			instrPointer += 19;
		}

		else if (instr == 10) // calc
		{
			if (instrPointer + 30 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}

			OpNumber opNum = (OpNumber)mem[instrPointer + 1];
			DatatypeNumber dataType = (DatatypeNumber)mem[instrPointer + 2];
			uint8_t size = datatypeSizes[(uint8_t)dataType];

			if (opNum == OpNumber::BOOL_NOT || opNum == OpNumber::BIT_NOT)
			{
				uint64_t addr1 = instrPointer + 3;
				uint64_t resAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3 + size));

				Math(opNum, dataType, addr1, 0, resAddr);
				instrPointer += 11 + size;
			}
			else
			{
				uint64_t addr1 = instrPointer + 3;
				uint64_t addr2 = instrPointer + 3 + size;
				uint64_t resAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3 + size * 2));

				//newTerm->Println();
				//ShowBytes(size, addr1);
				//newTerm->Println();
				//ShowBytes(size, addr2);
				//newTerm->Println();
				//ShowBytes(size, instrPointer + 3 + size * 2);
				//newTerm->Println();
				//ShowBytes(size, resAddr);
				//newTerm->Println();


				Math(opNum, dataType, addr1, addr2, resAddr);
				instrPointer += 11 + size * 2;
			}
		}

		else if (instr == 20) //jump
		{
			if (instrPointer + 8 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 1));

			instrPointer = jumpAddr;
		}
		else if (instr == 40) //if jump
		{
			if (instrPointer + 9 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			bool jump = *((char*)((uint64_t)mem + instrPointer + 1));;
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 2));

			if (jump)
				instrPointer = jumpAddr;
			else
				instrPointer += 10;
		}

		else if (instr == 25) //sub
		{
			if (instrPointer + 8 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 1));

			if (subDeepness > maxSubDeepness)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SUBROUTINE STACK OVERFLOW!";
				return;
			}

			subLastPos[subDeepness++] = instrPointer + 9;
			instrPointer = jumpAddr;
		}
		else if (instr == 30) //return
		{
			if (subDeepness < 1)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SUBROUTINE STACK UNDERFLOW!";
				return;
			}
			instrPointer = subLastPos[--subDeepness];
		}
		else if (instr == 45) //if_sub
		{
			if (instrPointer + 9 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			bool jump = *((char*)((uint64_t)mem + instrPointer + 1));;
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 2));

			if (subDeepness > maxSubDeepness)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SUBROUTINE STACK OVERFLOW!";
				return;
			}

			subLastPos[subDeepness++] = instrPointer + 10;
			instrPointer = jumpAddr;
		}


		else if (instr == 50) //syscall
		{
			if (instrPointer + 2 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint8_t syscall1 = mem[instrPointer + 1];
			uint8_t syscall2 = mem[instrPointer + 2];

			if (syscall1 == 1)
			{
				if (syscall2 == 1)
				{
					uint8_t byteToPrint = *((uint8_t*)((uint64_t)mem + instrPointer + 3));
					newTerm->Print((char)byteToPrint);
					instrPointer += 3 + 1;
				}
				else if (syscall2 == 2)
				{
					DatatypeNumber dataType1 = (DatatypeNumber)mem[instrPointer + 3];
					uint8_t size1 = datatypeSizes[(uint8_t)dataType1];

					uint64_t addr1 = instrPointer + 4;

					PrintVal(dataType1, addr1);
					instrPointer += 3 + 1 + size1;
				}
				else if (syscall2 == 3)
				{
					uint64_t printAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3));
					//newTerm->Println("<PRINTING STR AT {}>", to_string(printAddr), defCol);
					for (int i = 0; printAddr + i < memLen && mem[printAddr + i] != 0; i++)
					{
						newTerm->Print((char)mem[printAddr + i]);
					}
					instrPointer += 3 + 8;
				}
				else if (syscall2 == 4)
				{
					uint64_t rAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3));

					//newTerm->Println("\n<FREE GO BRRT>");

					//bool fRes = memHandler->FreeMem((void*)fAddr);

					if (rAddr + 8 >= memLen)
					{
						programEnded = true;
						errCode = 1;
						errMsg = "ADDRESS OUT OF BOUNDS!";
						return;
					}

					instrPointer += 3 + 8;
					gotInput = false;
					waitInput = true;
					writeInputInto = rAddr;
					break;
				}

				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "CONSOLE SYSCALL IS NOT SUPPORTED";
					return;
				}
			}
			else if (syscall1 == 2)
			{
				if (syscall2 == 1)
				{
					uint32_t mSize = *((uint32_t*)((uint64_t)mem + instrPointer + 3));
					uint64_t mAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 7));

					if (mAddr + 8 >= memLen)
					{
						programEnded = true;
						errCode = 1;
						errMsg = "ADDRESS OUT OF BOUNDS!";
						return;
					}
					
					//newTerm->Println("\n<MALLOC GO BRRT>");

					void* mRes = memHandler->MallocMem(mSize);
					
					if (mRes == NULL)
					{
						programEnded = true;
						errCode = 2;
						errMsg = "MALLOC FAILED!";
						return;
					}

					*((uint64_t*)(mem + mAddr)) = (uint64_t)mRes;


					instrPointer += 3 + 12;
				}
				else if (syscall2 == 2)
				{
					uint64_t fAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3));

					//newTerm->Println("\n<FREE GO BRRT>");

					bool fRes = memHandler->FreeMem((void*)fAddr);

					if (!fRes)
					{
						programEnded = true;
						errCode = 2;
						errMsg = "FREE FAILED!";
						return;
					}

					instrPointer += 3 + 8;
				}



				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "MEMORY SYSCALL IS NOT SUPPORTED";
					return;
				}
			}



			else
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SYSCALL IS NOT SUPPORTED";
				return;
			}

			//uint64_t jumpAddr = (uint64_t)((void*)mem[instrPointer + 1]);

			//instrPointer = jumpAddr;
		}



		else
		{
			newTerm->Print("\n<INVALID OPCODE: {}!>\n", to_string(instr), defCol);
			instrPointer++;
		}

	}



}

void TaskMAAB::PrintVal(DatatypeNumber numType, uint64_t numAddr)
{
	//newTerm->Println("\n<PRINT>\n", to_string(0), defCol);
	//newTerm->Println("+ TYPE: {}", to_string((uint8_t)numType), defCol);
	//newTerm->Println("+ SIZE: {}", to_string(datatypeSizes[(uint8_t)numType]), defCol);
	//newTerm->Println("+ ADDR: {}", to_string((uint64_t)numAddr), defCol);

	//newTerm->Print("DATA: ");
	//ShowBytes(datatypeSizes[(uint8_t)numType], numAddr);
	//newTerm->Println();

	if (numAddr + datatypeSizes[(uint8_t)numType] >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "ADDR OUT OF BOUNDS!";
		return;
	}

	if (numType == DatatypeNumber::CHAR)
	{
		char chr = (char)mem[numAddr];
		newTerm->Print(chr);
	}
	else
	{
		numAddr += (uint64_t)mem;
		if (numType == DatatypeNumber::INT)
			newTerm->Print(to_string(*((int32_t*)numAddr)));//*((int32_t*)((uint64_t)mem + addrFrom)) = from;
		else if (numType == DatatypeNumber::UINT)
			newTerm->Print(to_string(*((uint64_t*)numAddr)));

		else if (numType == DatatypeNumber::SHORT)
			newTerm->Print(to_string(*((int16_t*)numAddr)));
		else if (numType == DatatypeNumber::USHORT)
			newTerm->Print(to_string(*((uint16_t*)numAddr)));

		else if (numType == DatatypeNumber::LONG)
			newTerm->Print(to_string(*((int64_t*)numAddr)));
		else if (numType == DatatypeNumber::ULONG)
			newTerm->Print(to_string(*((uint64_t*)numAddr)));

		else if (numType == DatatypeNumber::BOOL)
		{
			if (*((bool*)numAddr))
				newTerm->Print("true");
			else
				newTerm->Print("false");
		}

		else if (numType == DatatypeNumber::FLOAT)
			newTerm->Print(to_string(*((float*)numAddr)));
		else if (numType == DatatypeNumber::DOUBLE)
			newTerm->Print(to_string(*((double*)numAddr)));
	}
}

void TaskMAAB::ShowBytes(uint8_t amount, uint64_t addr)
{
	newTerm->Print("<");
	for (int i = 0; i < amount; i++)
		newTerm->Print("{} ", to_string(*((uint8_t*)((uint64_t)mem + addr + i))));
	newTerm->Print(">");
}

void TaskMAAB::Cast(DatatypeNumber typeFrom, uint64_t addrFrom, DatatypeNumber typeTo, uint64_t addrTo)
{
	//newTerm->Println("\n<CAST>\n", to_string(0), defCol);
	//newTerm->Println("+ FROM:      {}", to_string((uint8_t)typeFrom), defCol);
	//newTerm->Println("+ FROM SIZE: {}", to_string(datatypeSizes[(uint8_t)typeFrom]), defCol);
	//newTerm->Println("+ FROM ADDR: {}", to_string((uint64_t)addrFrom), defCol);
	//newTerm->Println();
	//newTerm->Println("+ TO:        {}", to_string((uint8_t)typeTo), defCol);
	//newTerm->Println("+ TO ADDR:   {}", to_string((uint64_t)addrTo), defCol);
	//newTerm->Println("+ TO SIZE:   {}", to_string(datatypeSizes[(uint8_t)typeTo]), defCol);

	//newTerm->Print("FROM: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeFrom], addrFrom);
	//newTerm->Println();

	if (addrFrom + datatypeSizes[(uint8_t)typeFrom] >= memLen ||
		addrTo + datatypeSizes[(uint8_t)typeTo] >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "ADDR OUT OF BOUNDS!";
		return;
	}

#pragma region INT
	if (typeFrom == DatatypeNumber::INT)
	{
		int32_t from = *((int32_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region UINT
	else if (typeFrom == DatatypeNumber::UINT)
	{
		uint32_t from = *((uint32_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region SHORT
	else if (typeFrom == DatatypeNumber::SHORT)
	{
		int16_t from = *((int16_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region USHORT
	else if (typeFrom == DatatypeNumber::USHORT)
	{
		uint16_t from = *((uint16_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region LONG
	else if (typeFrom == DatatypeNumber::LONG)
	{
		int64_t from = *((int64_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region ULONG
	else if (typeFrom == DatatypeNumber::ULONG)
	{
		uint64_t from = *((uint64_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region FLOAT
	else if (typeFrom == DatatypeNumber::FLOAT)
	{
		float from = *((float*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region DOUBLE
	else if (typeFrom == DatatypeNumber::DOUBLE)
	{
		double from = *((double*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region CHAR
	else if (typeFrom == DatatypeNumber::CHAR)
	{
		char from = *((char*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region BOOL
	else if (typeFrom == DatatypeNumber::BOOL)
	{
		bool from = *((bool*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion

	else
	{
		programEnded = true;
		errCode = 1;
		errMsg = "UNKNOWN DATATYPE!";
		return;
	}


	//newTerm->Print("TO: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeTo], addrTo);
	//newTerm->Println();
}

void TaskMAAB::Math(OpNumber opNum, DatatypeNumber typeNum, uint64_t addr1, uint64_t addr2, uint64_t addrRes)
{
	if (addr1 + datatypeSizes[(uint8_t)typeNum] >= memLen ||
		addr1 + datatypeSizes[(uint8_t)typeNum] >= memLen ||
		addrRes + datatypeSizes[(uint8_t)typeNum] >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "ADDR OUT OF BOUNDS!";
		return;
	}


	//newTerm->Println("\n<MATH>\n", to_string(0), defCol);
	//newTerm->Println("+ OP TYPE:  {}", to_string((uint8_t)opNum), defCol);
	//newTerm->Println("+ NUM TYPE: {}", to_string((uint8_t)typeNum), defCol);
	//newTerm->Println("+ NUM SIZE: {}", to_string(datatypeSizes[(uint8_t)typeNum]), defCol);
	//newTerm->Println("+ ADDR 1:   {}", to_string((uint64_t)addr1), defCol);
	//newTerm->Println("+ ADDR 2:   {}", to_string((uint64_t)addr2), defCol);
	//newTerm->Println("+ ADDR RES: {}", to_string((uint64_t)addrRes), defCol);


	//newTerm->Print("DATA ADDR 1: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeNum], addr1);
	//newTerm->Println();
	//newTerm->Print("DATA ADDR 2: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeNum], addr2);
	//newTerm->Println();


	addr1 += (uint64_t)mem;
	addr2 += (uint64_t)mem;
	addrRes += (uint64_t)mem;


#pragma region INT
	if (typeNum == DatatypeNumber::INT)
	{
		int32_t a = *((int32_t*)addr1);
		int32_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int32_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int32_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int32_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int32_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int32_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int32_t*)addrRes) = a % b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;
		else if (opNum == OpNumber::BIT_AND)
			*((int32_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int32_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int32_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int32_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int32_t*)addrRes) = a >> b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region UINT
	else if (typeNum == DatatypeNumber::UINT)
	{
		uint32_t a = *((uint32_t*)addr1);
		uint32_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((uint32_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((uint32_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((uint32_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((uint32_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((uint32_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((uint32_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((uint32_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((uint32_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((uint32_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((uint32_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((uint32_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region SHORT
	else if (typeNum == DatatypeNumber::SHORT)
	{
		int16_t a = *((int16_t*)addr1);
		int16_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int16_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int16_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int16_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int16_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int16_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int16_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((int16_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int16_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int16_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int16_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int16_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region USHORT
	else if (typeNum == DatatypeNumber::USHORT)
	{
		uint16_t a = *((uint16_t*)addr1);
		uint16_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((uint16_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((uint16_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((uint16_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((uint16_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((uint16_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((uint16_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((uint16_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((uint16_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((uint16_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((uint16_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((uint16_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region LONG
	else if (typeNum == DatatypeNumber::LONG)
	{
		int64_t a = *((int64_t*)addr1);
		int64_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int64_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int64_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int64_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int64_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int64_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int64_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((int64_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int64_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int64_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int64_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int64_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region ULONG
	else if (typeNum == DatatypeNumber::ULONG)
	{
		uint64_t a = *((uint64_t*)addr1);
		uint64_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((uint64_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((uint64_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((uint64_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((uint64_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((uint64_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((uint64_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((uint64_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((uint64_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((uint64_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((uint64_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((uint64_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region FLOAT
	else if (typeNum == DatatypeNumber::FLOAT)
	{
		float a = *((float*)addr1);
		float b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((float*)addr2);

		if (opNum == OpNumber::PLUS)
			*((float*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((float*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((float*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
		{
			if (b != 0)
				*((float*)addrRes) = a / b;
		}
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region DOUBLE
	else if (typeNum == DatatypeNumber::DOUBLE)
	{
		double a = *((double*)addr1);
		double b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((double*)addr2);

		if (opNum == OpNumber::PLUS)
			*((double*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((double*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((double*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
		{
			if (b != 0)
				*((double*)addrRes) = a / b;
		}
		else if (opNum == OpNumber::EQUALS)
			*((double*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((double*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((double*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((double*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((double*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((double*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion


#pragma region CHAR
	else if (typeNum == DatatypeNumber::CHAR)
	{
		int8_t a = *((int8_t*)addr1);
		int8_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int8_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int8_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int8_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int8_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int8_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int8_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((int8_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int8_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int8_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int8_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int8_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;


		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region BOOL
	else if (typeNum == DatatypeNumber::BOOL)
	{
		bool a = *((bool*)addr1);
		bool b = false;
		if (opNum != OpNumber::BOOL_NOT)
			b = *((bool*)addr2);

		if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::BOOL_AND)
			*((bool*)addrRes) = a && b;
		else if (opNum == OpNumber::BOOL_OR)
			*((bool*)addrRes) = a || b;
		else if (opNum == OpNumber::BOOL_NOT)
			*((bool*)addrRes) = !a;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

	else
	{
		programEnded = true;
		errCode = 1;
		errMsg = "UNKNOWN DATATYPE!";
		return;
	}


	//newTerm->Print("DATA RES:    ");
	//ShowBytes(8, addrRes - (uint64_t)mem);
	//newTerm->Println();
}




void TaskMAAB::Free()
{
	free((void*)mem);
	free((void*)memHandler);
}


TaskMAAB* NewMAABTask(uint32_t codeLen, uint8_t* code, Window* window, TerminalInstance* newTerm)
{
	//TaskMAAB* maab = (TaskMAAB*)malloc(sizeof(TaskMAAB));
	//*maab = TaskMAAB(codeLen, code, window, newTerm);
	TaskMAAB* maab = new TaskMAAB(codeLen, code, window, newTerm);
	return maab;
}