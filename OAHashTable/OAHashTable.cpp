// Assignment : OAHashTable
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023
#include <cstring>

/*
copy config
init stats
table size <- GetClosestPrime to initial table size
create table buffer with table size
if there is a 2nd hash function
    delete policy must be MARK

Initialize Table
 */
template <typename T>
OAHashTable<T>::OAHashTable(const OAHTConfig& Config)
    : config_(Config)
{
    stats_.TableSize_ = GetClosestPrime(config_.InitialTableSize_);
    stats_.PrimaryHashFunc_ = config_.PrimaryHashFunc_;
    stats_.SecondaryHashFunc_ = config_.SecondaryHashFunc_;

    table_ = new OAHTSlot[stats_.TableSize_];

    if (stats_.SecondaryHashFunc_ != nullptr)
    {
        config_.DeletionPolicy_ = OAHTDeletionPolicy::MARK;
    }
}

template <typename T>
OAHashTable<T>::~OAHashTable()
{
    clear();
    delete[] table_;
}

/*calculate load factor
    GrowTable if load factor is bigger than max load factor or if item count is table size

    index, slot <- Get Index And Slot of key

    if index shows that key is already in table
        throw Duplicate exception
    slot State <- OCCUPIED
    slot Key <- key
    slot Data <- data

    update item count*/
template <typename T>
void OAHashTable<T>::insert([[maybe_unused]] const char* Key, [[maybe_unused]] const T& Data)
{
    auto load_factor = (stats_.Count_ + 1) / static_cast<double>(stats_.TableSize_);

    if (load_factor > config_.MaxLoadFactor_ || stats_.Count_ == stats_.TableSize_)
    {
        GrowTable();
    }

    OAHTSlot* slot;
    int index = IndexOf(Key, slot);

    if (index != -1)
    {
        throw OAHashTableException(OAHashTableException::E_DUPLICATE, "Key already exists in the table");
    }

    slot->State = OAHTSlot::OCCUPIED;
    strncpy(slot->Key, Key, MAX_KEYLEN - 1);
    slot->Data = Data;

    stats_.Count_++;
}

/*index, slot <- Get Index And Slot of key
    if index shows that the key is not in the table
        throw exception ITEM_NOT_FOUND

    update item count

    free slot data if given a free callback

    if MARK policy
        slot state <- DELETED
    else PACK policy
        slot state <- UNOCCUPIED
        start index <- index
        increment index
        wrap around index if it is larger than table size
        slot <- slot at new index

        while slot state is OCCUPIED and index is not start index
            slot state <- UNOCCUPIED
            update item count
            current key <- copy of slot key
            insert (current key, slot Data)
            increment index
            wrap around index if it is larger than table size
            slot <- slot at new index*/
template <typename T>
void OAHashTable<T>::remove([[maybe_unused]] const char* Key)
{
}

/*index, slot <- Get Index And Slot of key
    if index shows that the key is not in the table
        throw exception ITEM_NOT_FOUND
    return slot data*/
template <typename T>
const T& OAHashTable<T>::find([[maybe_unused]] const char* Key) const
{
    OAHTSlot* slot;
    int index = IndexOf(Key, slot);

    if (index == -1)
    {
        throw OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Item not found.");
    }

    return slot->Data;
}

/*if there is a free callback
        for each OCCUPIED slot
            free data
    Initialize Table*/
template <typename T>
void OAHashTable<T>::clear()
{
    if (config_.FreeProc_ != nullptr)
    {
        for (unsigned i = 0; i < stats_.TableSize_; ++i)
        {
            if (table_[i].State == OAHTSlot::OCCUPIED)
            {
                config_.FreeProc_(table_[i].Data);
            }
        }
        InitTable();
    }
}

template <typename T>
OAHTStats OAHashTable<T>::GetStats() const
{
    return stats_;
}

template <typename T>
const typename OAHashTable<T>::OAHTSlot* OAHashTable<T>::GetTable() const
{
    return table_;
}

/*for each slot in table
        set state to be UNOCCUPIED
    item count <- 0*/
template <typename T>
void OAHashTable<T>::InitTable()
{
    for (unsigned i = 0; i < stats_.TableSize_; ++i)
    {
        table_[i].State = OAHTSlot::UNOCCUPIED;
    }
    stats_.Count_ = 0;
}

/*new size <- ceil of current table size multiply by growth factor
    new size <- GetClosestPrime to new size
    old table <- current table
    old table size <- current table size
    set table to buffer with new table size
    if failed to create new buffer
        throw NO MEMORY exception
    table size <- new size
    for each OCCUPIED slot in old table
        insert (current key, current value)
    delete old table
    update number of expansions*/
template <typename T>
void OAHashTable<T>::GrowTable()
{
}

/*slot <- null
    index <- 1st hash of key with table size
    start index <- index

    if using 2nd hash
        stride <- 1 + 2nd hash of key with (table size - 1)
    else linear probing
        stride <- 1

    slot probe count <- 0

    while not done
        Update global probe count
        Update slot probe count
        current slot <- slot at index
        if current slot is DELETED and slot is null
            slot <- current slot
        if current slot is UNOCCUPIED
            if slot is null
                slot <- current slot
            save slot probe count to slot
            return {-1, slot}
        else if current slot is Not DELETED
            if current slot key equals key
                slot <- current slot
                save slot probe count to slot
                return {index, slot}
        index <- index + stride
        wrap around index if it is larger than table size
        if index equals start index
            done <- true

    return {-1, slot}*/
template <typename T>
int OAHashTable<T>::IndexOf(const char* Key, OAHTSlot*& Slot) const
{
    Slot = nullptr;
    int index = static_cast<int>(config_.PrimaryHashFunc_(Key, stats_.TableSize_));
    int start_index = index;
    int stride;

    if (config_.SecondaryHashFunc_)
        stride = 1 + static_cast<int>(config_.SecondaryHashFunc_(Key, stats_.TableSize_ - 1));
    else
        stride = 1;

    int slot_probes = 0;
    bool done = false;

    while (!done)
    {
        stats_.Probes_++;
        slot_probes++;

        OAHTSlot& curr_slot = table_[index];

        if (curr_slot.State == OAHTSlot::DELETED && Slot == nullptr)
            Slot = &curr_slot;

        if (curr_slot.State == OAHTSlot::UNOCCUPIED)
        {
            if (Slot == nullptr)
                Slot = &curr_slot;
            Slot->probes = slot_probes;
            return -1;
        }
        else if (curr_slot.State != OAHTSlot::DELETED)
        {
            if (strcmp(curr_slot.Key, Key) == 0)
            {
                Slot = &curr_slot;
                Slot->probes = slot_probes;
                return index;
            }
        }

        index = (index + stride);

        if (index == start_index)
            done = true;
    }

    return -1;
}