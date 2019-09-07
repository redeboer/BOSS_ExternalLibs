
#include "DummyLoadOldROOTAlg.h"
#include "GaudiKernel/MsgStream.h"

#include "TFile.h"
#include "TSystem.h"

DummyLoadOldROOTAlg::DummyLoadOldROOTAlg(const std::string& name, ISvcLocator* pSvcLocator)
	: Algorithm(name, pSvcLocator) 
{
	declareProperty("OldRootList", m_string_vec);
}

StatusCode DummyLoadOldROOTAlg::initialize() {
  MsgStream log(msgSvc(), name());

	std::vector<std::string>::iterator it;
	for (it = m_string_vec.begin(); it != m_string_vec.end(); ++it) {
		TString tmpfn = (*it).c_str();
		gSystem->ExpandPathName(tmpfn);

		TFile* tmp = new TFile(tmpfn);
		log << MSG::DEBUG << "Try to Load File: " << *it << endreq;
		if (!tmp) {
			log << MSG::WARNING << "Load File Failed: " << *it << endreq;
			continue;
		}
	}
	return StatusCode::SUCCESS;
}

StatusCode DummyLoadOldROOTAlg::execute() {
	return StatusCode::SUCCESS;
}

StatusCode DummyLoadOldROOTAlg::finalize() {
	return StatusCode::SUCCESS;
}
