// Assignment : OAHashTable
// Name : Wonhyeong Chae
// Course : CS280
// Term : Spring 2023
#include <cstring>
#include <cmath>

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
    InitTable();
}

template <typename T>
OAHashTable<T>::~OAHashTable()
{
    clear();
    delete[] table_;
}

template <typename T>
void OAHashTable<T>::insert([[maybe_unused]] const char* Key, [[maybe_unused]] const T& Data)
{
    double load_factor = static_cast<double>(stats_.Count_ + 1) / stats_.TableSize_;
    if (load_factor > config_.MaxLoadFactor_ || stats_.Count_ == stats_.TableSize_)
    {
        GrowTable();
    }

    OAHTSlot* slot;
    int index = IndexOf(Key, slot);

    if (index != -1)
    {
        throw OAHashTableException(OAHashTableException::E_DUPLICATE, "Duplicate key.");
    }

    slot->State = OAHTSlot::OCCUPIED;
    strncpy(slot->Key, Key, MAX_KEYLEN - 1);
    slot->Data = Data;

    stats_.Count_++;
}

template <typename T>
void OAHashTable<T>::remove([[maybe_unused]] const char* Key)
{
    OAHTSlot* slot;
    int index = IndexOf(Key, slot);

    if (index == -1)
    {
        throw OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Item not found.");
    }

    stats_.Count_--;

    if (config_.FreeProc_ != nullptr)
    {
        config_.FreeProc_(slot->Data);
    }

    if (config_.DeletionPolicy_ == MARK)
    {
        slot->State = OAHTSlot::DELETED;
    }
    else
    {
        slot->State = OAHTSlot::UNOCCUPIED;

        int start_index = index;
        index = (index + 1) % static_cast<int>(stats_.TableSize_);
        slot = &table_[index];

        while (slot->State == OAHTSlot::OCCUPIED && index != start_index)
        {
            slot->State = OAHTSlot::UNOCCUPIED;
            stats_.Count_--;

            char current_key[MAX_KEYLEN];
            strncpy(current_key, slot->Key, MAX_KEYLEN - 1);

            insert(current_key, slot->Data);

            index = (index + 1) % static_cast<int>(stats_.TableSize_);
            slot = &table_[index];
        }
    }
}

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

template <typename T>
void OAHashTable<T>::clear()
{
    if (config_.FreeProc_)
    {
        for (unsigned i = 0; i < stats_.TableSize_; ++i)
        {
            if (table_[i].State == OAHTSlot::OCCUPIED)
            {
                config_.FreeProc_(table_[i].Data);
            }
        }
    }
    InitTable();
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

template <typename T>
void OAHashTable<T>::InitTable()
{
    for (unsigned i = 0; i < stats_.TableSize_; ++i)
    {
        table_[i].State = OAHTSlot::UNOCCUPIED;
    }
    stats_.Count_ = 0;
}

template <typename T>
void OAHashTable<T>::GrowTable()
{
    unsigned new_size = static_cast<unsigned>(ceil(stats_.TableSize_ * config_.GrowthFactor_));
    new_size = GetClosestPrime(new_size);

    OAHTSlot* old_table = table_;
    unsigned oldTableSize = stats_.TableSize_;

    try
    {
        table_ = new OAHTSlot[new_size];
    }
    catch (std::bad_alloc& e)
    {
        throw e.what();
    }

    stats_.TableSize_ = new_size;

    this->InitTable();

    for (unsigned i = 0; i < oldTableSize; ++i)
    {
        if (old_table[i].State == OAHTSlot::OCCUPIED)
        {
            insert(old_table[i].Key, old_table[i].Data);
        }
    }

    delete[] old_table;

    stats_.Expansions_++;
}

template <typename T>
int OAHashTable<T>::IndexOf(const char* Key, OAHTSlot*& Slot) const
{
    Slot = nullptr;
    int index = static_cast<int>(stats_.PrimaryHashFunc_(Key, stats_.TableSize_));
    int start_index = index;
    int stride;

    if (stats_.SecondaryHashFunc_)
    {
        stride = 1 + static_cast<int>(stats_.SecondaryHashFunc_(Key, stats_.TableSize_ - 1));
    }
    else
    {
        stride = 1;
    }

    int slot_probe_count = 0;
    bool done = false;

    while (!done)
    {
        stats_.Probes_++;
        slot_probe_count++;

        OAHTSlot& current_slot = table_[index];

        if (current_slot.State == OAHTSlot::DELETED && !Slot)
        {
            Slot = &current_slot;
        }
        if (current_slot.State == OAHTSlot::UNOCCUPIED)
        {
            if (Slot == nullptr)
            {
                Slot = &current_slot;
            }
            Slot->probes = slot_probe_count;
            return -1;
        }
        else if (current_slot.State != OAHTSlot::DELETED)
        {
            if (strncmp(current_slot.Key, Key, MAX_KEYLEN - 1) == 0)
            {
                Slot = &current_slot;
                Slot->probes = slot_probe_count;
                return index;
            }
        }

        index = (index + stride) % static_cast<int>(stats_.TableSize_);

        if (index == start_index)
        {
            done = true;
        }
    }

    return -1;
}