/*++ NDK Version: 0098

Copyright (c) Alex Ionescu.  All rights reserved.

Header Name:

    mmtypes.h

Abstract:

    Type definitions for the Memory Manager

Author:

    Alex Ionescu (alexi@tinykrnl.org) - Updated - 27-Feb-2006

--*/

#ifndef _MMTYPES_H
#define _MMTYPES_H

//
// Dependencies
//
#include <umtypes.h>
#include <arch/mmtypes.h>
#include <extypes.h>

//
// Page-Rounding Macros
//
#define PAGE_ROUND_DOWN(x)                                  \
    (((ULONG_PTR)(x))&(~(PAGE_SIZE-1)))
#define PAGE_ROUND_UP(x)                                    \
    ( (((ULONG_PTR)(x)) + PAGE_SIZE-1)  & (~(PAGE_SIZE-1)) )
#ifdef NTOS_MODE_USER
#define ROUND_TO_PAGES(Size)                                \
    (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#endif
#define ROUND_TO_ALLOCATION_GRANULARITY(Size)               \
    (((ULONG_PTR)(Size) + MM_ALLOCATION_GRANULARITY - 1)    \
    & ~(MM_ALLOCATION_GRANULARITY - 1))

//
// Macro for generating pool tags
//
#define TAG(A, B, C, D)                                     \
    (ULONG)(((A)<<0) + ((B)<<8) + ((C)<<16) + ((D)<<24))

//
// PFN Identity Uses
//
#define MMPFNUSE_PROCESSPRIVATE                             0
#define MMPFNUSE_FILE                                       1
#define MMPFNUSE_PAGEFILEMAPPED                             2
#define MMPFNUSE_PAGETABLE                                  3
#define MMPFNUSE_PAGEDPOOL                                  4
#define MMPFNUSE_NONPAGEDPOOL                               5
#define MMPFNUSE_SYSTEMPTE                                  6
#define MMPFNUSE_SESSIONPRIVATE                             7
#define MMPFNUSE_METAFILE                                   8
#define MMPFNUSE_AWEPAGE                                    9
#define MMPFNUSE_DRIVERLOCKPAGE                             10
#define MMPFNUSE_KERNELSTACK                                11

#ifndef NTOS_MODE_USER

//
// Virtual Memory Flags
//
#define MEM_WRITE_WATCH                                     0x200000
#define MEM_PHYSICAL                                        0x400000
#define MEM_ROTATE                                          0x800000
#define MEM_IMAGE                                           SEC_IMAGE
#define MEM_DOS_LIM                                         0x40000000

//
// Section Flags for NtCreateSection
//
#define SEC_NO_CHANGE                                       0x400000
#define SEC_FILE                                            0x800000
#define SEC_IMAGE                                           0x1000000
#define SEC_PROTECTED_IMAGE                                 0x2000000
#define SEC_RESERVE                                         0x4000000
#define SEC_COMMIT                                          0x8000000
#define SEC_NOCACHE                                         0x10000000
#define SEC_WRITECOMBINE                                    0x40000000
#define SEC_LARGE_PAGES                                     0x80000000
#else
#define SEC_BASED                                           0x200000

//
// Section Inherit Flags for NtCreateSection
//
typedef enum _SECTION_INHERIT
{
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

//
// Pool Types
//
typedef enum _POOL_TYPE
{
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType,
    NonPagedPoolSession = 32,
    PagedPoolSession,
    NonPagedPoolMustSucceedSession,
    DontUseThisTypeSession,
    NonPagedPoolCacheAlignedSession,
    PagedPoolCacheAlignedSession,
    NonPagedPoolCacheAlignedMustSSession
} POOL_TYPE;
#endif

//
// Memory Manager Page Lists
//
typedef enum _MMLISTS
{
   ZeroedPageList = 0,
   FreePageList = 1,
   StandbyPageList = 2,
   ModifiedPageList = 3,
   ModifiedNoWritePageList = 4,
   BadPageList = 5,
   ActiveAndValid = 6,
   TransitionPage = 7
} MMLISTS;

//
// Per Processor Non Paged Lookaside List IDs
//
typedef enum _PP_NPAGED_LOOKASIDE_NUMBER
{
    LookasideSmallIrpList = 0,
    LookasideLargeIrpList = 1,
    LookasideMdlList = 2,
    LookasideCreateInfoList = 3,
    LookasideNameBufferList = 4,
    LookasideTwilightList = 5,
    LookasideCompletionList = 6,
    LookasideMaximumList = 7
} PP_NPAGED_LOOKASIDE_NUMBER;

//
// Memory Information Classes for NtQueryVirtualMemory
//
typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation,
    MemoryWorkingSetList,
    MemorySectionName,
    MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS;

//
// Section Information Clasess for NtQuerySection
//
typedef enum _SECTION_INFORMATION_CLASS
{
    SectionBasicInformation,
    SectionImageInformation,
} SECTION_INFORMATION_CLASS;

#ifdef NTOS_MODE_USER

//
// Virtual Memory Counters
//
typedef struct _VM_COUNTERS
{
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} VM_COUNTERS, *PVM_COUNTERS;

typedef struct _VM_COUNTERS_EX
{
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivateUsage;
} VM_COUNTERS_EX, *PVM_COUNTERS_EX;
#endif

//
// Sub-Information Types for PFN Identity
//
typedef struct _MEMORY_FRAME_INFORMATION
{
    ULONGLONG UseDescription:4;
    ULONGLONG ListDescription:3;
    ULONGLONG Reserved0:1;
    ULONGLONG Pinned:1;
    ULONGLONG DontUse:48;
    ULONGLONG Priority:3;
    ULONGLONG Reserved:4;
} MEMORY_FRAME_INFORMATION, *PMEMORY_FRAME_INFORMATION;

typedef struct _FILEOFFSET_INFORMATION
{
    ULONGLONG DontUse:9;
    ULONGLONG Offset:48;
    ULONGLONG Reserved:7;
} FILEOFFSET_INFORMATION, *PFILEOFFSET_INFORMATION;

typedef struct _PAGEDIR_INFORMATION
{
    ULONGLONG DontUse:9;
    ULONGLONG PageDirectoryBase:48;
    ULONGLONG Reserved:7;
} PAGEDIR_INFORMATION, *PPAGEDIR_INFORMATION;

typedef struct _UNIQUE_PROCESS_INFORMATION
{
    ULONGLONG DontUse:9;
    ULONGLONG UniqueProcessKey:48;
    ULONGLONG Reserved:7;
} UNIQUE_PROCESS_INFORMATION, *PUNIQUE_PROCESS_INFORMATION;

//
// PFN Identity Data Structure
//
typedef struct _MMPFN_IDENTITY
{
    union
    {
        MEMORY_FRAME_INFORMATION e1;
        FILEOFFSET_INFORMATION e2;
        PAGEDIR_INFORMATION e3;
        UNIQUE_PROCESS_INFORMATION e4;
    } u1;
    SIZE_T PageFrameIndex;
    union
    {
        struct
        {
            ULONG Image:1;
            ULONG Mismatch:1;
        } e1;
        PVOID FileObject;
        PVOID UniqueFileObjectKey;
        PVOID ProtoPteAddress;
        PVOID VirtualAddress;
    } u2;
} MMPFN_IDENTITY, *PMMPFN_IDENTITY;

//
// List of Working Sets
//
typedef struct _MEMORY_WORKING_SET_LIST
{
    ULONG NumberOfPages;
    ULONG WorkingSetList[1];
} MEMORY_WORKING_SET_LIST, *PMEMORY_WORKING_SET_LIST;

//
// Memory Information Structures for NtQueryVirtualMemory
//
typedef struct
{
    UNICODE_STRING SectionFileName;
    WCHAR NameBuffer[ANYSIZE_ARRAY];
} MEMORY_SECTION_NAME, *PMEMORY_SECTION_NAME;

//
// Section Information Structures for NtQuerySection
//
typedef struct _SECTION_BASIC_INFORMATION
{
    PVOID           BaseAddress;
    ULONG           Attributes;
    LARGE_INTEGER   Size;
} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

typedef struct _SECTION_IMAGE_INFORMATION
{
    PVOID TransferAddress;
    ULONG ZeroBits;
    SIZE_T MaximumStackSize;
    SIZE_T CommittedStackSize;
    ULONG SubSystemType;
    union
    {
        struct
        {
            USHORT SubSystemMinorVersion;
            USHORT SubSystemMajorVersion;
        };
        ULONG SubSystemVersion;
    };
    union
    {
        struct
        {
            USHORT MajorOperatingSystemVersion;
            USHORT MinorOperatingSystemVersion;
        };
        ULONG OperatingSystemVersion;
    };
    USHORT ImageCharacteristics;
    USHORT DllCharacteristics;
    USHORT Machine;
    BOOLEAN ImageContainsCode;
    union
    {
        UCHAR ImageFlags;
        struct
        {
            UCHAR ComPlusNativeReady : 1;
            UCHAR ComPlusILOnly : 1;
            UCHAR ImageDynamicallyRelocated : 1;
            UCHAR ImageMappedFlat : 1;
            UCHAR BaseBelow4gb : 1;
            UCHAR ComPlusPrefer32bit : 1;
            UCHAR Reserved : 2;
        };
    };
    ULONG LoaderFlags;
    ULONG ImageFileSize;
    ULONG CheckSum;
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

#ifndef NTOS_MODE_USER

//
// PTE Structures
//
typedef struct _MMPTE
{
    union
    {
        ULONG Long;
        HARDWARE_PTE Flush;
        MMPTE_HARDWARE Hard;
        MMPTE_PROTOTYPE Proto;
        MMPTE_SOFTWARE Soft;
        MMPTE_TRANSITION Trans;
        MMPTE_SUBSECTION Subsect;
        MMPTE_LIST List;
    } u;
} MMPTE, *PMMPTE;

//
// Section Extension Information
//
typedef struct _MMEXTEND_INFO
{
    ULONGLONG CommittedSize;
    ULONG ReferenceCount;
} MMEXTEND_INFO, *PMMEXTEND_INFO;

//
// Segment and Segment Flags
//
typedef struct _SEGMENT_FLAGS
{
    ULONG TotalNumberOfPtes4132:10;
    ULONG ExtraSharedWowSubsections:1;
    ULONG LargePages:1;
    ULONG Spare:20;
} SEGMENT_FLAGS, *PSEGMENT_FLAGS;

typedef struct _SEGMENT
{
    struct _CONTROL_AREA *ControlArea;
    ULONG TotalNumberOfPtes;
    ULONG NonExtendedPtes;
    ULONG Spare0;
    ULONGLONG SizeOfSegment;
    MMPTE SegmentPteTemplate;
    ULONG NumberOfCommittedPages;
    PMMEXTEND_INFO ExtendInfo;
    SEGMENT_FLAGS SegmentFlags;
    PVOID BaseAddress;
    union
    {
        SIZE_T ImageCommitment;
        PEPROCESS CreatingProcess;
    } u1;
    union
    {
        PSECTION_IMAGE_INFORMATION ImageInformation;
        PVOID FirstMappedVa;
    } u2;
    PMMPTE PrototypePte;
    MMPTE ThePtes[1];
} SEGMENT, *PSEGMENT;

//
// Event Counter Structure
//
typedef struct _EVENT_COUNTER
{
    ULONG RefCount;
    KEVENT Event;
    LIST_ENTRY ListEntry;
} EVENT_COUNTER, *PEVENT_COUNTER;

//
// Flags
//
typedef struct _MMSECTION_FLAGS
{
    ULONG BeingDeleted:1;
    ULONG BeingCreated:1;
    ULONG BeingPurged:1;
    ULONG NoModifiedWriting:1;
    ULONG FailAllIo:1;
    ULONG Image:1;
    ULONG Based:1;
    ULONG File:1;
    ULONG Networked:1;
    ULONG NoCache:1;
    ULONG PhysicalMemory:1;
    ULONG CopyOnWrite:1;
    ULONG Reserve:1;
    ULONG Commit:1;
    ULONG FloppyMedia:1;
    ULONG WasPurged:1;
    ULONG UserReference:1;
    ULONG GlobalMemory:1;
    ULONG DeleteOnClose:1;
    ULONG FilePointerNull:1;
    ULONG DebugSymbolsLoaded:1;
    ULONG SetMappedFileIoComplete:1;
    ULONG CollidedFlush:1;
    ULONG NoChange:1;
    ULONG filler0:1;
    ULONG ImageMappedInSystemSpace:1;
    ULONG UserWritable:1;
    ULONG Accessed:1;
    ULONG GlobalOnlyPerSession:1;
    ULONG Rom:1;
    ULONG WriteCombined:1;
    ULONG filler:1;
} MMSECTION_FLAGS, *PMMSECTION_FLAGS;

typedef struct _MMSUBSECTION_FLAGS
{
    ULONG ReadOnly:1;
    ULONG ReadWrite:1;
    ULONG SubsectionStatic:1;
    ULONG GlobalMemory:1;
    ULONG Protection:5;
    ULONG Spare:1;
    ULONG StartingSector4132:10;
    ULONG SectorEndOffset:12;
} MMSUBSECTION_FLAGS, *PMMSUBSECTION_FLAGS;

typedef struct _MMSUBSECTION_FLAGS2
{
    ULONG SubsectionAccessed:1;
    ULONG SubsectionConverted:1;
    ULONG Reserved:30;
} MMSUBSECTION_FLAGS2;

//
// Control Area Structures
//
typedef struct _CONTROL_AREA
{
    PSEGMENT Segment;
    LIST_ENTRY DereferenceList;
    ULONG NumberOfSectionReferences;
    ULONG NumberOfPfnReferences;
    ULONG NumberOfMappedViews;
    ULONG NumberOfSystemCacheViews;
    ULONG NumberOfUserReferences;
    union
    {
        ULONG LongFlags;
        MMSECTION_FLAGS Flags;
    } u;
    PFILE_OBJECT FilePointer;
    PEVENT_COUNTER WaitingForDeletion;
    USHORT ModifiedWriteCount;
    USHORT FlushInProgressCount;
    ULONG WritableUserReferences;
    ULONG QuadwordPad;
} CONTROL_AREA, *PCONTROL_AREA;

typedef struct _LARGE_CONTROL_AREA
{
    PSEGMENT Segment;
    LIST_ENTRY DereferenceList;
    ULONG NumberOfSectionReferences;
    ULONG NumberOfPfnReferences;
    ULONG NumberOfMappedViews;
    ULONG NumberOfSystemCacheViews;
    ULONG NumberOfUserReferences;
    union
    {
        ULONG LongFlags;
        MMSECTION_FLAGS Flags;
    } u;
    PFILE_OBJECT FilePointer;
    PEVENT_COUNTER WaitingForDeletion;
    USHORT ModifiedWriteCount;
    USHORT FlushInProgressCount;
    ULONG WritableUserReferences;
    ULONG QuadwordPad;
    ULONG StartingFrame;
    LIST_ENTRY UserGlobalList;
    ULONG SessionId;
} LARGE_CONTROL_AREA, *PLARGE_CONTROL_AREA;

//
// Subsection and Mapped Subsection
//
typedef struct _SUBSECTION
{
    PCONTROL_AREA ControlArea;
    union
    {
        ULONG LongFlags;
        MMSUBSECTION_FLAGS SubsectionFlags;
    } u;
    ULONG StartingSector;
    ULONG NumberOfFullSectors;
    PMMPTE SubsectionBase;
    ULONG UnusedPtes;
    ULONG PtesInSubsection;
    struct _SUBSECTION *NextSubsection;
} SUBSECTION, *PSUBSECTION;

typedef struct _MSUBSECTION
{
    PCONTROL_AREA ControlArea;
    union
    {
        ULONG LongFlags;
        MMSUBSECTION_FLAGS SubsectionFlags;
    } u;
    ULONG StartingSector;
    ULONG NumberOfFullSectors;
    PMMPTE SubsectionBase;
    ULONG UnusedPtes;
    ULONG PtesInSubsection;
    struct _SUBSECTION *NextSubsection;
    LIST_ENTRY DereferenceList;
    ULONG_PTR NumberOfMappedViews;
    union
    {
        ULONG LongFlags2;
        MMSUBSECTION_FLAGS2 SubsectionFlags2;
    } u2;
} MSUBSECTION, *PMSUBSECTION;

//
// Segment Object
//
typedef struct _SEGMENT_OBJECT
{
    PVOID BaseAddress;
    ULONG TotalNumberOfPtes;
    LARGE_INTEGER SizeOfSegment;
    ULONG NonExtendedPtes;
    ULONG ImageCommitment;
    PCONTROL_AREA ControlArea;
    PSUBSECTION Subsection;
    PLARGE_CONTROL_AREA LargeControlArea;
    PMMSECTION_FLAGS MmSectionFlags;
    PMMSUBSECTION_FLAGS MmSubSectionFlags;
} SEGMENT_OBJECT, *PSEGMENT_OBJECT;

//
// Section Object
//
typedef struct _SECTION_OBJECT
{
    PVOID StartingVa;
    PVOID EndingVa;
    PVOID LeftChild;
    PVOID RightChild;
    PSEGMENT_OBJECT Segment;
} SECTION_OBJECT, *PSECTION_OBJECT;

//
// Generic Address Range Structure
//
typedef struct _ADDRESS_RANGE
{
    ULONG BaseAddrLow;
    ULONG BaseAddrHigh;
    ULONG LengthLow;
    ULONG LengthHigh;
    ULONG Type;
} ADDRESS_RANGE, *PADDRESS_RANGE;

//
// Node in Memory Manager's AVL Table
//
typedef struct _MMADDRESS_NODE
{
    union
    {
        ULONG Balance:2;
        struct _MMADDRESS_NODE *Parent;
    } u1;
    struct _MMADDRESS_NODE *LeftChild;
    struct _MMADDRESS_NODE *RightChild;
    ULONG StartingVpn;
    ULONG EndingVpn;
} MMADDRESS_NODE, *PMMADDRESS_NODE;

//
// Memory Manager AVL Table for VADs and other descriptors
//
typedef struct _MM_AVL_TABLE
{
    MMADDRESS_NODE BalancedRoot;
    ULONG DepthOfTree:5;
    ULONG Unused:3;
    ULONG NumberGenericTableElements:24;
    PVOID NodeHint;
    PVOID NodeFreeHint;
} MM_AVL_TABLE, *PMM_AVL_TABLE;

//
// Actual Section Object
//
typedef struct _SECTION
{
    MMADDRESS_NODE Address;
    PSEGMENT Segment;
    LARGE_INTEGER SizeOfSection;
    union
    {
        ULONG LongFlags;
        MMSECTION_FLAGS Flags;
    } u;
    ULONG InitialPageProtection;
} SECTION, *PSECTION;

//
// Memory Manager Working Set Structures
//
typedef struct _MMWSLENTRY
{
    ULONG Valid:1;
    ULONG LockedInWs:1;
    ULONG LockedInMemory:1;
    ULONG Protection:5;
    ULONG Hashed:1;
    ULONG Direct:1;
    ULONG Age:2;
    ULONG VirtualPageNumber:14;
} MMWSLENTRY, *PMMWSLENTRY;

typedef struct _MMWSLE
{
    union
    {
        PVOID VirtualAddress;
        ULONG Long;
        MMWSLENTRY e1;
    } u1;
} MMWSLE, *PMMWSLE;

typedef struct _MMWSLE_HASH
{
    PVOID Key;
    ULONG Index;
} MMWSLE_HASH, *PMMWSLE_HASH;

typedef struct _MMWSL
{
    ULONG FirstFree;
    ULONG FirstDynamic;
    ULONG LastEntry;
    ULONG NextSlot;
    PMMWSLE Wsle;
    ULONG LastInitializedWsle;
    ULONG NonDirectCount;
    PMMWSLE_HASH HashTable;
    ULONG HashTableSize;
    ULONG NumberOfCommittedPageTables;
    PVOID HashTableStart;
    PVOID HighestPermittedHashAddress;
    ULONG NumberOfImageWaiters;
    ULONG VadBitMapHint;
    USHORT UsedPageTableEntries[768];
    ULONG CommittedPageTables[24];
} MMWSL, *PMMWSL;

//
// Flags for Memory Support Structure
//
typedef struct _MMSUPPORT_FLAGS
{
    ULONG SessionSpace:1;
    ULONG BeingTrimmed:1;
    ULONG SessionLeader:1;
    ULONG TrimHard:1;
    ULONG MaximumWorkingSetHard:1;
    ULONG ForceTrim:1;
    ULONG MinimumworkingSetHard:1;
    ULONG Available0:1;
    ULONG MemoryPriority:8;
    ULONG GrowWsleHash:1;
    ULONG AcquiredUnsafe:1;
    ULONG Available:14;
} MMSUPPORT_FLAGS, *PMMSUPPORT_FLAGS;

//
// Per-Process Memory Manager Data
//
typedef struct _MMSUPPORT
{
#if (NTDDI_VERSION >= NTDDI_WS03)
    LIST_ENTRY WorkingSetExpansionLinks;
#endif
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    USHORT LastTrimpStamp;
    USHORT NextPageColor;
#else
    LARGE_INTEGER LastTrimTime;
#endif
    MMSUPPORT_FLAGS Flags;
    ULONG PageFaultCount;
    ULONG PeakWorkingSetSize;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    ULONG Spare0;
#else
    ULONG GrowthSinceLastEstimate;
#endif
    ULONG MinimumWorkingSetSize;
    ULONG MaximumWorkingSetSize;
    PMMWSL VmWorkingSetList;
#if (NTDDI_VERSION < NTDDI_WS03)
    LIST_ENTRY WorkingSetExpansionLinks;
#endif
    ULONG Claim;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    ULONG Spare;
    ULONG WorkingSetPrivateSize;
    ULONG WorkingSetSizeOverhead;
#else
    ULONG NextEstimationSlot;
    ULONG NextAgingSlot;
    ULONG EstimatedAvailable;
#endif
    ULONG WorkingSetSize;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    PKEVENT ExitEvent;
#endif
    EX_PUSH_LOCK WorkingSetMutex;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    PVOID AccessLog;
#endif
} MMSUPPORT, *PMMSUPPORT;

//
// Memory Information Types
//
typedef struct _MEMORY_BASIC_INFORMATION
{
    PVOID BaseAddress;
    PVOID AllocationBase;
    ULONG AllocationProtect;
    ULONG RegionSize;
    ULONG State;
    ULONG Protect;
    ULONG Type;
} MEMORY_BASIC_INFORMATION,*PMEMORY_BASIC_INFORMATION;

//
// Driver Verifier Data
//
typedef struct _MM_DRIVER_VERIFIER_DATA
{
    ULONG Level;
    ULONG RaiseIrqls;
    ULONG AcquireSpinLocks;
    ULONG SynchronizeExecutions;
    ULONG AllocationsAttempted;
    ULONG AllocationsSucceeded;
    ULONG AllocationsSucceededSpecialPool;
    ULONG AllocationsWithNoTag;
    ULONG TrimRequests;
    ULONG Trims;
    ULONG AllocationsFailed;
    ULONG AllocationsFailedDeliberately;
    ULONG Loads;
    ULONG Unloads;
    ULONG UnTrackedPool;
    ULONG UserTrims;
    ULONG CurrentPagedPoolAllocations;
    ULONG CurrentNonPagedPoolAllocations;
    ULONG PeakPagedPoolAllocations;
    ULONG PeakNonPagedPoolAllocations;
    ULONG PagedBytes;
    ULONG NonPagedBytes;
    ULONG PeakPagedBytes;
    ULONG PeakNonPagedBytes;
    ULONG BurstAllocationsFailedDeliberately;
    ULONG SessionTrims;
    ULONG Reserved[2];
} MM_DRIVER_VERIFIER_DATA, *PMM_DRIVER_VERIFIER_DATA;

//
// Internal Driver Verifier Table Data
//
typedef struct _DRIVER_SPECIFIED_VERIFIER_THUNKS
{
    LIST_ENTRY ListEntry;
    struct _LDR_DATA_TABLE_ENTRY *DataTableEntry;
    ULONG NumberOfThunks;
} DRIVER_SPECIFIED_VERIFIER_THUNKS, *PDRIVER_SPECIFIED_VERIFIER_THUNKS;

//
// Default heap size values.  For user mode, these values are copied to a new
// process's PEB by the kernel in MmCreatePeb.  In kernel mode, RtlCreateHeap
// reads these variables directly.
//
// These variables should be considered "const"; they are written only once,
// during MmInitSystem.
//
extern SIZE_T MmHeapSegmentReserve;
extern SIZE_T MmHeapSegmentCommit;
extern SIZE_T MmHeapDeCommitTotalFreeThreshold;
extern SIZE_T MmHeapDeCommitFreeBlockThreshold;

//
// Section Object Type
//
extern POBJECT_TYPE MmSectionObjectType;

#endif // !NTOS_MODE_USER

#endif // _MMTYPES_H
