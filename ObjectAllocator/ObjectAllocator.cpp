#include "ObjectAllocator.h"

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
	:  PageList_(nullptr), FreeList_(nullptr), config_(config)
{
	stats_.ObjectSize_ = ObjectSize;
	stats_.PageSize_ = config_.ObjectsPerPage_ * ObjectSize + sizeof(void*);

	try
	{
		CreatePage();
	}
	catch(OAException& error)
	{
		throw(error);
	}
}

ObjectAllocator::~ObjectAllocator()
{
	char* buffer;

	while (PageList_ != nullptr)
	{
		buffer = reinterpret_cast<char*>(PageList_);
		PageList_ = PageList_->Next;
		delete[] buffer;
	}
}

void ObjectAllocator::CreatePage()
{
	char* buffer;

	try
	{
		buffer = new char[stats_.PageSize_];
	}
	catch (std::bad_alloc&)
	{
		throw(OAException(OAException::E_NO_MEMORY, "Not enough memory."));
	}

	PageList_ = reinterpret_cast<GenericObject*>(buffer);
	buffer += sizeof(void*);
	stats_.PagesInUse_++;

	for (unsigned i = 0; i < config_.ObjectsPerPage_; i++)
	{
		GenericObject* list = FreeList_;

		FreeList_ = reinterpret_cast<GenericObject*>(buffer);
		stats_.FreeObjects_++;
		FreeList_->Next = list;

		buffer += stats_.ObjectSize_;
	}
}

void* ObjectAllocator::Allocate([[maybe_unused]]const char* label)
{
	if (PageList_ == nullptr)
	{
		try
		{
			CreatePage();
		}
		catch(OAException& error)
		{
			throw(error);
		}
	}
	else if (FreeList_ == nullptr)
	{
		if (config_.MaxPages_ == 0 || stats_.PagesInUse_ < config_.MaxPages_)
			CreatePage();
		else
			throw(OAException(OAException::E_NO_PAGES, "No more pages can be allocated."));
	}

	char* buffer2 = reinterpret_cast<char*>(FreeList_);

	FreeList_ = FreeList_->Next;

	stats_.ObjectsInUse_++;

	if (stats_.ObjectsInUse_ > stats_.MostObjects_)
		stats_.MostObjects_++;

	stats_.Allocations_++;
	stats_.FreeObjects_--;
	
	return buffer2;
}

void ObjectAllocator::Free(void* Object)
{
	GenericObject* block = reinterpret_cast<GenericObject*>(Object);
	block->Next = FreeList_;
	FreeList_ = block;

	--stats_.ObjectsInUse_;
	++stats_.Deallocations_;
	++stats_.FreeObjects_;
}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
	
	return 1;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
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
