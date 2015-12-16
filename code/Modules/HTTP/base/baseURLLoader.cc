//------------------------------------------------------------------------------
//  baseURLLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "baseURLLoader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
baseURLLoader::putRequest(const Ptr<HTTPProtocol::HTTPRequest>& req) {
    o_assert(req.isValid());
    this->requestQueue.Enqueue(req);
}

//------------------------------------------------------------------------------
void
baseURLLoader::doWork() {
    // override this method in platform-specific subclass
    o_error("baseURLLoader::doWork() called!\n");
}

//------------------------------------------------------------------------------
bool
baseURLLoader::handleCancelled(const Ptr<HTTPProtocol::HTTPRequest>& httpReq) {
    const auto& ioReq = httpReq->IoRequest;
    if (ioReq.isValid()) {
        if (ioReq->Cancelled()) {
            ioReq->Status = IOStatus::Cancelled;
            ioReq->SetHandled();
            httpReq->SetCancelled();
            httpReq->SetHandled();
            return true;
        }
    }
    if (httpReq->Cancelled()) {
        httpReq->SetHandled();
        return true;
    }
    return false;
}

} // namespace _priv
} // namespace Oryol