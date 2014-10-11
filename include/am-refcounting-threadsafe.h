// vim: set sts=8 ts=2 sw=2 tw=99 et:
//
// Copyright (C) 2013, David Anderson and AlliedModders LLC
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//  * Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//  * Neither the name of AlliedModders LLC nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef _include_amtl_ts_refcounting_h_
#define _include_amtl_ts_refcounting_h_

#include <am-refcounting.h>
#include <am-atomics.h>

namespace ke {

// See the comment above Refcounted<T> for more information. This class is
// identical, except changing the reference count is guaranteed to be atomic
// with respect to other threads changing the reference count.
template <typename T>
class KE_LINK RefcountedThreadsafe
{
  public:
    RefcountedThreadsafe()
     : refcount_(0)
    {
    }

    void AddRef() {
      refcount_.increment();
    }
    bool Release() {
        if (!refcount_.decrement()) {
            delete static_cast<T *>(this);
            return false;
        }
        return true;
    }

  protected:
    ~RefcountedThreadsafe() {
    }

  private:
    AtomicRefcount refcount_;
};

// Classes may be multiply-inherited may wish to derive from this Refcounted
// instead.
class VirtualRefcountedThreadsafe : public IRefcounted
{
 public:
  VirtualRefcountedThreadsafe() : refcount_(0)
  {
#if !defined(NDEBUG)
    destroying_ = false;
#endif
  }
  virtual ~VirtualRefcountedThreadsafe()
  {}
  void AddRef() KE_OVERRIDE {
    assert(!destroying_);
    refcount_.increment();
  }
  void Release() KE_OVERRIDE {
    if (!refcount_.decrement()) {
#if !defined(NDEBUG)
      destroying_ = true;
#endif
      delete this;
    }
  }

 private:
  AtomicRefcount refcount_;
#if !defined(NDEBUG)
  bool destroying_;
#endif
};

} // namespace ke

#endif // _include_amtl_ts_refcounting_h_
