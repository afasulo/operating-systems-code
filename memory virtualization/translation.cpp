#include "src.hpp"
#include <math.h>
#include <string.h>

#define BITS_PER_BYTE 8

// Return the virtual page number associated with the given virtual address
void split_virtual_address(int virtual_address, int page_size, int* VPN, int* offset)
{
    // the number of bits for the offset is log2 of the page size
    int offset_bits = log2(page_size);
    // the offset is the lower 'offset_bits' of the virtual address
    *offset = virtual_address & (page_size -1);
    // The VPN is the remaining upper bits _>>>>
    *VPN = virtual_address >> offset_bits;
}



// Split VPN into index and tag
// Assuming k-way associative TLB
void split_VPN(int VPN, int k, int* index, int* tag)
{
    // the idnesx is the reainder of the VPN divided by the number of sets (k).
    *index = VPN % k;
    // the tag is the result of the integer divsion of the VPN by k
    *tag = VPN / k;
}





// Find PFN from TLB (if it exists and is accessible)
int TLB_lookup(TLB* tlb, int VPN)
{
    int index, tag;
    PTE* entry = NULL;

    // determine the index and tag from the VPN
    split_VPN(VPN, tlb->k, &index, &tag);

    // attempt to find the entry in the TLB. Throws C++ style exception on miss
    if ( !tlb -> lookup(index, tag, &entry) ) {
        throw TLB_MISS;
    }

    // check for protection faults
    if (entry -> protect_bit == 1) {
        protection_fault();
    }

    // on success return PFN
    return entry -> PFN;
}





// Find PFN in PageTable (if it exists and is accessible)
int table_lookup(PageTable* table, TLB* tlb, int VPN)
{
    PTE *entry = NULL;
    int index, tag;

    table -> lookup(VPN, &entry);

    //  We must first check if the entry is NULL. A NULL entry means the VPN
    // is not in the page table at all, which is a segmentation fault.
    if (entry == NULL || entry->valid_bit == 0) {
        segmentation_fault();
    }

    /* Check for protection faults. */
    if (entry->protect_bit == 1) {
        protection_fault();
    }

    /* If the present bit is 0, it's a page fault. */
    if (entry->present_bit == 0) {
        page_fault();
    }

    /* If the lookup is successful, add the entry to the TLB. */
    split_VPN(VPN, tlb->k, &index, &tag);
    tlb->add_entry(index, tag, entry);

    /* Return the PFN. */
    return entry->PFN;
}





// Convert PFN and Offset to Physical Address
int get_physical_address(int PFN, int offset, int page_size)
{
    int offset_bits = log2(page_size);

    // the physical addr is formed by shifting the PFN and adding the offset
    int physical_address = (PFN << offset_bits) | offset;
    return physical_address;
}





// Put everything together to get physical address from virtual
int virtual_to_physical(int virtual_address, int page_size, TLB* tlb, PageTable* table)
{
    int VPN, offset, PFN;

    // break down the virtual address into its VPN and offset
    split_virtual_address(virtual_address, page_size, &VPN, &offset);

    try {
        PFN = TLB_lookup(tlb, VPN);
    } catch (const char* msg) {
        //On a TLB miss, consult the page table
        if (strcmp(msg, TLB_MISS) == 0) {
            PFN = table_lookup(table, tlb, VPN);
        } else {
            //Propagate any other exceptions
            throw;
        }
    }

    return get_physical_address(PFN, offset, page_size);
}

