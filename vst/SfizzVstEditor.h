// SPDX-License-Identifier: BSD-2-Clause

// This code is part of the sfizz library and is licensed under a BSD 2-clause
// license. You should have receive a LICENSE.md file along with the code.
// If not, contact the sfizz maintainers at https://github.com/sfztools/sfizz

#pragma once
#include "SfizzVstController.h"
#include "editor/EditorController.h"
#include "public.sdk/source/vst/vstguieditor.h"
#include <mutex>
class Editor;
#if !defined(__APPLE__) && !defined(_WIN32)
namespace VSTGUI { class RunLoop; }
#endif

using namespace Steinberg;
using namespace VSTGUI;

class SfizzVstEditor : public Vst::VSTGUIEditor,
                       public EditorController {
public:
    explicit SfizzVstEditor(SfizzVstController* controller);
    ~SfizzVstEditor();

    bool PLUGIN_API open(void* parent, const VSTGUI::PlatformType& platformType = VSTGUI::kDefaultNative) override;
    void PLUGIN_API close() override;

    SfizzVstController* getController() const
    {
        return static_cast<SfizzVstController*>(Vst::VSTGUIEditor::getController());
    }

    // VSTGUIEditor
    CMessageResult notify(CBaseObject* sender, const char* message) override;

    //
    void updateState(const SfizzVstState& state);
    void updateUiState(const SfizzUiState& uiState);
    void updatePlayState(const SfizzPlayState& playState);
    SfizzUiState getCurrentUiState() const;
    void receiveMessage(const void* data, uint32_t size);

private:
    void processOscQueue();
    void flushOscQueue();

protected:
    // EditorController
    void uiSendValue(EditId id, const EditValue& v) override;
    void uiBeginSend(EditId id) override;
    void uiEndSend(EditId id) override;
    void uiSendMIDI(const uint8_t* data, uint32_t len) override;
    void uiSendMessage(const char* path, const char* sig, const sfizz_arg_t* args) override;

private:
    void loadSfzFile(const std::string& filePath);
    void loadScalaFile(const std::string& filePath);

    void updateStateDisplay();

    Vst::ParamID parameterOfEditId(EditId id);

    std::unique_ptr<Editor> editor_;

#if !defined(__APPLE__) && !defined(_WIN32)
    SharedPointer<RunLoop> _runLoop;
#endif

    // messaging
    std::unique_ptr<uint8[]> oscTemp_;

    // editor state
    // note: might be updated from a non-UI thread
    mutable std::recursive_mutex stateMutex_;
    SfizzVstState state_;
    SfizzUiState uiState_;
    SfizzPlayState playState_;
    volatile bool mustRedisplayState_ = false;
    volatile bool mustRedisplayUiState_ = false;
    volatile bool mustRedisplayPlayState_ = false;
    std::vector<uint8_t> oscQueue_;
};
