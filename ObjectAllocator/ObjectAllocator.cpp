#include "ObjectAllocator.h"
#include <cstring>

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
	: PageList_(nullptr), FreeList_(nullptr), config_(config)
{
	stats_.ObjectSize_ = ObjectSize;

	if (config_.Alignment_ != 0)
	{
		if (sizeof(void*) + config_.PadBytes_ + config_.HBlockInfo_.size_ <= config_.Alignment_)
		{
			config_.LeftAlignSize_ = config_.Alignment_ - config_.PadBytes_ - sizeof(void*) - static_cast<unsigned>(config_.HBlockInfo_.size_);
		}
		else
		{
			config_.LeftAlignSize_ = config_.Alignment_ * 2 - config_.PadBytes_ - sizeof(void*)
				- static_cast<unsigned>(config_.HBlockInfo_.size_);
		}

		if (config_.HBlockInfo_.size_ + config_.PadBytes_ * 2 < config_.Alignment_)
		{
			config_.InterAlignSize_ = config_.Alignment_ - static_cast<unsigned>(config_.HBlockInfo_.size_)
				- config_.PadBytes_ * 2;
		}
		else
		{
			config_.InterAlignSize_ = config_.Alignment_ * 2 - static_cast<unsigned>(config_.HBlockInfo_.size_)
				- config_.PadBytes_ * 2;
		}
	}

	stats_.PageSize_ = sizeof(void*) + config_.LeftAlignSize_;
	stats_.PageSize_ += config_.ObjectsPerPage_ * (config_.PadBytes_ * 2 + ObjectSize + config_.HBlockInfo_.size_);
	stats_.PageSize_ += (config_.ObjectsPerPage_ - 1) * config_.InterAlignSize_;

	if (config_.UseCPPMemManager_ == false)
	{
		try
		{
			CreatePage();
		}
		catch (OAException& error)
		{
			throw(error);
		}
	}
}

ObjectAllocator::~ObjectAllocator() throw()
{
	char* buffer;

	while (PageList_ != nullptr)
	{
		buffer = reinterpret_cast<char*>(PageList_);
		PageList_ = PageList_->Next;
		delete[] buffer;
	}
}

void ObjectAllocator::CreatePage(GenericObject* nextPage)
{
	char* buffer = new char[stats_.PageSize_];

	PageList_ = reinterpret_cast<GenericObject*>(buffer);
	PageList_->Next = nextPage;
	buffer += sizeof(void*);
	stats_.PagesInUse_++;

	for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
	{
		if (i == 0)
		{
			if (config_.DebugOn_ == true)
			{
				std::memset(buffer, ALIGN_PATTERN, config_.LeftAlignSize_);
			}
			if (i < config_.ObjectsPerPage_ - 1)
			{
				buffer += config_.LeftAlignSize_;
			}
		}
		else
		{
			if (config_.DebugOn_ == true)
			{
				std::memset(buffer, ALIGN_PATTERN, config_.InterAlignSize_);
			}
			buffer += config_.InterAlignSize_;
		}

		if (config_.DebugOn_)
		{
			std::memset(buffer, 0, config_.HBlockInfo_.size_);
		}

		buffer += config_.HBlockInfo_.size_;

		if (config_.DebugOn_)
		{
			std::memset(buffer, PAD_PATTERN, config_.PadBytes_);
		}

		buffer += config_.PadBytes_;

		if (config_.DebugOn_)
		{
			std::memset(buffer, UNALLOCATED_PATTERN, stats_.ObjectSize_);
		}

		GenericObject* list = FreeList_;
		FreeList_ = reinterpret_cast<GenericObject*>(buffer);
		stats_.FreeObjects_++;
		FreeList_->Next = list;

		buffer += stats_.ObjectSize_;

		if (config_.DebugOn_)
		{
			std::memset(buffer, PAD_PATTERN, config_.PadBytes_);
		}

		buffer += config_.PadBytes_;
	}
}

void* ObjectAllocator::Allocate([[maybe_unused]] const char* label)
{
	if (config_.UseCPPMemManager_)
	{
		char* buffer;
		try
		{
			buffer = new char[stats_.ObjectSize_];
		}
		catch(std::bad_alloc &)
		{
			throw(OAException(OAException::E_NO_MEMORY, "Not enough memory."));
		}

		++stats_.ObjectsInUse_;
		if (stats_.ObjectsInUse_ > stats_.MostObjects_)
		{
			++stats_.MostObjects_;
		}
		++stats_.Allocations_;
		--stats_.FreeObjects_;
		return buffer;
	}

	if (FreeList_ == nullptr)
	{
		if (stats_.PagesInUse_ < config_.MaxPages_)
			CreatePage(PageList_);
		else
		{
			if (config_.DebugOn_)
				throw(OAException(OAException::E_NO_PAGES, "No more pages can be allocated."));
		}
	}

	stats_.ObjectsInUse_++;

	if (FreeList_ == nullptr)
		return nullptr;

	char* buffer = reinterpret_cast<char*>(FreeList_);

	FreeList_ = FreeList_->Next;

	if (config_.DebugOn_)
	{
		std::memset(buffer, ALLOCATED_PATTERN, stats_.ObjectSize_);
	}

	if (config_.HBlockInfo_.size_ != 0)
	{
		char* header = buffer - config_.PadBytes_ - sizeof(char);
		*header = static_cast<char>(1);
	}

	if (stats_.ObjectsInUse_ > stats_.MostObjects_)
		stats_.MostObjects_++;

	stats_.Allocations_++;
	stats_.FreeObjects_--;

	return buffer;
}

void ObjectAllocator::Free(void* Object)
{
	if (config_.UseCPPMemManager_)
	{
		auto block = reinterpret_cast<char*>(Object);
		delete[] block;

		stats_.ObjectsInUse_--;
		stats_.Deallocations_++;
		stats_.FreeObjects_++;

		return;
	}

	GenericObject* page = PageList_;
	auto obj = reinterpret_cast<char*>(Object);

	if (config_.DebugOn_)
	{
		bool isBound = false;
		bool isCorrupted = false;
		bool isFreed = true;

		while (page != nullptr)
		{
			if (reinterpret_cast<char*>(page) < obj && obj < reinterpret_cast<char*>(page) + stats_.PageSize_)
			{
				char* blockStart = reinterpret_cast<char*>(page) + sizeof(void*) + config_.LeftAlignSize_ + config_.PadBytes_ + config_.HBlockInfo_.size_;
				size_t separation = stats_.ObjectSize_ + config_.PadBytes_ * 2 + config_.InterAlignSize_ + config_.HBlockInfo_.size_;

				// Check if object is on a block boundary
				if (0 == (obj - blockStart) % separation)
				{
					isBound = true;
				}
			}
			page = page->Next;
		}

		if (isBound == false)
		{
			throw(OAException(OAException::E_BAD_BOUNDARY, "Object not on a block boundary"));
		}
		else
		{
			// Check data block
			for (unsigned i = sizeof(char*); i < stats_.ObjectSize_; ++i)
			{
				if (obj[i] != FREED_PATTERN)
				{
					isFreed = false;
					break;
				}
			}

			// Check padding blocks
			char* leftPad = obj - config_.PadBytes_;
			char* rightPad = obj + stats_.ObjectSize_;
			for (unsigned i = 0; i < config_.PadBytes_; ++i)
			{
				if (leftPad[i] != PAD_PATTERN || rightPad[i] != PAD_PATTERN)
				{
					isCorrupted = true;
					break;
				}
			}
		}

		if (isFreed == true)
		{
			throw(OAException(OAException::E_MULTIPLE_FREE, "Block already freed"));
		}
		else if (isCorrupted == true)
		{
			throw(OAException(OAException::E_CORRUPTED_BLOCK, "Block is corrupted"));
		}
	}

	if (config_.HBlockInfo_.size_ != 0)
	{
		char* header = obj - config_.PadBytes_ - sizeof(char);
		*header = 0;
	}

	if (config_.DebugOn_)
	{
		std::memset(Object, FREED_PATTERN, stats_.ObjectSize_);
	}

	auto block = reinterpret_cast<GenericObject*>(Object);
	block->Next = FreeList_;
	FreeList_ = block;

	stats_.ObjectsInUse_--;
	stats_.Deallocations_++;
	stats_.FreeObjects_++;
}

unsigned ObjectAllocator::DumpMemoryInUse([[maybe_unused]] DUMPCALLBACK fn) const
{
	return 1;
}

unsigned ObjectAllocator::ValidatePages([[maybe_unused]] VALIDATECALLBACK fn) const
{
	return 1;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
	return 1;
}

bool ObjectAllocator::ImplementedExtraCredit()
{
	return false;
}

void ObjectAllocator::SetDebugState(bool State)
{
	config_.DebugOn_ = State;
}

const void* ObjectAllocator::GetFreeList() const
{
	return FreeList_;
}

const void* ObjectAllocator::GetPageList() const
{
	return PageList_;
}

OAConfig ObjectAllocator::GetConfig() const
{
	return config_;
}

OAStats ObjectAllocator::GetStats() const
{
	return stats_;
}
