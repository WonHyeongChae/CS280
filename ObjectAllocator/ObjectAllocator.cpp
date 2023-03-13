#include "ObjectAllocator.h"

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
{
	
}

ObjectAllocator::~ObjectAllocator()
{
}

void* ObjectAllocator::Allocate(const char* label)
{
	return FreeList_;
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
	return true;
}

void ObjectAllocator::SetDebugState(bool State)
{
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
	return OAConfig();
}

OAStats ObjectAllocator::GetStats() const
{
	return OAStats();
}
