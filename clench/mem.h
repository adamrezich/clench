#pragma once
#include <vector>
#include <queue>
#include <iostream>

namespace clench {

  namespace mem {

    struct GenericHandle {
      bool invalid = true;
      inline void Invalidate() {
        invalid = true;
      }
    };

    template<typename T, size_t TBlocksPerPage = 64>
    struct Managed {

      bool invalid;

      struct Page {
        Page *nextPage = nullptr;
        std::queue<size_t> vacancies;
        size_t nextBlock = 0;

        T blocks[TBlocksPerPage];
      };

      static Page firstPage;
      static size_t pageCount;

      static const size_t blockSize;
      static const size_t blocksPerPage;

    public:
      virtual ~Managed() {}
      virtual void Cleanup() = 0;

      size_t uid;
      size_t page;
      size_t block;

      struct Handle : GenericHandle {

        size_t uid;
        size_t page;
        size_t block;
        bool invalid;

        inline bool operator==(const Handle &rhs) const {
          if (invalid || rhs.invalid)
            throw std::exception();

          return uid == rhs.uid;
        }
        inline bool operator!=(const Handle &rhs) const {
          if (invalid || rhs.invalid)
            throw std::exception();

          return uid != rhs.uid;
        }

        Handle() : invalid(true) {}

        Handle(size_t uid_, size_t page_, size_t block_) : uid(uid_), page(page_), block(block_), invalid(false) {}

        T &operator*() {
          T &obj = Managed<T, TBlocksPerPage>::GetPage(page)->blocks[block];
          if (obj.invalid || obj.uid != uid) {
            invalid = true;
          }
          return obj;
        }

        T *operator->() {
          T *obj = &Managed<T, TBlocksPerPage>::GetPage(page)->blocks[block];
          if (obj->invalid || obj->uid != uid) {
            invalid = true;
          }
          return obj;
        }

        Handle &operator=(const Handle &rhs) const {
          if (rhs.invalid)
            throw std::exception();
          obj = rhs.obj;
          uid = rhs.obj;
          page = rhs.obj;
          block = rhs.obj;
        }

        inline bool IsValid() {
          T *obj = &Managed<T, TBlocksPerPage>::GetPage(page)->blocks[block];
          if (obj->invalid || obj->uid != uid) {
            invalid = true;
          }
          return !invalid;
        }

      };

      static void Initialize();

      static Handle Allocate() {

        // make sure we're initialized
        static bool initialized = false;
        if (!initialized) {
          Initialize();
          initialized = true;
        }

        // start at the first page each time
        Page *page = &firstPage;
        size_t pageNum = 0;

        // find the last page with vacancies, and if there isn't one, make one
        while (page->vacancies.empty() && page->nextBlock == TBlocksPerPage) {
          ++pageNum;
          if (!page->nextPage) {
            ++pageCount;
            page->nextPage = new Page();
          }
          page = page->nextPage;
        }

        // now that we've found our page, find the block we want
        size_t block = 0;
        if (!page->vacancies.empty()) {
          block = page->vacancies.front();
          page->vacancies.pop();
        }
        else {
          block = page->nextBlock++;
        }

        T *allocated = new (page->blocks + block) T;
        static size_t nextuid = 0;
#ifdef max
#undef max
#endif
        if (nextuid == std::numeric_limits<std::size_t>::max())
          throw std::exception("well shit son, we're out of uids");
        allocated->uid = nextuid;
        allocated->page = pageNum;
        allocated->block = block;
        return Handle(nextuid++, pageNum, block);
      }

      inline static Page *GetPage(size_t num) {
        Page *page = &firstPage;
        while (num-- > 0) {
          if (page->nextPage == nullptr)
            throw std::exception();
          page = page->nextPage;
        }
        return page;
      }

    protected:
      inline void Invalidate() {
        invalid = true;
      }

    public:
      void Free();
      Handle GetHandle();
      inline bool IsInvalid() const {
        return invalid;
      }
    };

    /* god I hate C++ */
    template<typename T, size_t TBlocksPerPage>
    size_t Managed<T, TBlocksPerPage>::pageCount = 1;

    template<typename T, size_t TBlocksPerPage>
    typename Managed<T, TBlocksPerPage>::Page Managed<T, TBlocksPerPage>::firstPage;

    template<typename T, size_t TBlocksPerPage>
    const size_t Managed<T, TBlocksPerPage>::blockSize = sizeof(T);

    template<typename T, size_t TBlocksPerPage>
    const size_t Managed<T, TBlocksPerPage>::blocksPerPage = TBlocksPerPage;

    template<typename T, size_t TBlocksPerPage>
    void Managed<T, TBlocksPerPage>::Initialize() {
    }

    template<typename T, size_t TBlocksPerPage>
    void Managed<T, TBlocksPerPage>::Free() {
      Cleanup();
      GetPage(page)->vacancies.push(uid);
      Invalidate();
    }

    template<typename T, size_t TBlocksPerPage>
    typename Managed<T, TBlocksPerPage>::Handle Managed<T, TBlocksPerPage>::GetHandle() {
      if (invalid)
        throw std::exception();
      return Handle(uid, page, block);
    }

  }

}
