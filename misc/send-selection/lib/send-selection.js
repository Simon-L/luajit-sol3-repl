'use babel';

import SendSelectionView from './send-selection-view';
import { CompositeDisposable } from 'atom';

export default {

  sendSelectionView: null,
  modalPanel: null,
  subscriptions: null,

  activate(state) {
    this.sendSelectionView = new SendSelectionView(state.sendSelectionViewState);
    this.modalPanel = atom.workspace.addModalPanel({
      item: this.sendSelectionView.getElement(),
      visible: false
    });

    // Events subscribed to in atom's system can be easily cleaned up with a CompositeDisposable
    this.subscriptions = new CompositeDisposable();

    this.subscriptions.add(atom.commands.add('atom-workspace', {
      'send-selection:send': () => this.send()
    }));

    console.log("activated");
  },

  send() {
    const editor = atom.workspace.getActiveTextEditor()
    if (editor) {
      const selection = editor.getSelectedBufferRanges()
      console.log(selection);
      editor.getSelections().forEach((item, i) => {
          console.log(item.getText())
      });

      console.log(editor.getSelectedBufferRanges().toString());
    }
  },

  deactivate() {
    this.modalPanel.destroy();
    this.subscriptions.dispose();
    this.sendSelectionView.destroy();
    console.log("deactivated");
  },

  serialize() {
    return {
      sendSelectionViewState: this.sendSelectionView.serialize()
    };
  },

};
