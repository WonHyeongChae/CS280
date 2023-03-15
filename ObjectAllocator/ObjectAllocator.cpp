#include "ObjectAllocator.h"

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
	: config_(config), stats_(), PageList_(nullptr), FreeList_(nullptr)
{
	stats_.ObjectSize_ = ObjectSize;
	stats_.PageSize_ = config_.ObjectsPerPage_ * ObjectSize + sizeof(void*);
}

ObjectAllocator::~ObjectAllocator()
{
}

void ObjectAllocator::CreatePage()
{
	char* buffer = new char[stats_.PageSize_];

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
		CreatePage();
	else if (FreeList_ == nullptr)
	{
		if (config_.MaxPages_ == 0 || stats_.PagesInUse_ < config_.MaxPages_)
			CreatePage();
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
