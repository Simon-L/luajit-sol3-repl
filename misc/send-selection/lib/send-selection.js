'use babel';

import SendSelectionView from './send-selection-view';
import { CompositeDisposable } from 'atom';

const get = require('simple-get')

var config = require('../config.json')

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

    this.subscriptions = new CompositeDisposable();

    this.subscriptions.add(atom.commands.add('atom-workspace', {
      'send-selection:send-selection': () => this.send_selection(),
      'send-selection:send-line': () => this.send_line()
    }));
  },

  send_line() {
    const editor = atom.workspace.getActiveTextEditor()
    if (editor) {
      const p = editor.getCursorBufferPosition()
      const line = editor.lineTextForBufferRow(p.row)
      this.sendhttp(line)
    }

  },

  send_selection() {
    const editor = atom.workspace.getActiveTextEditor()
    if (editor) {
      editorView = atom.views.getView(editor)
      atom.commands.dispatch(editorView, 'editor:select-line')
      const selection = editor.getSelectedBufferRanges()
      code = ""
      editor.getSelections().forEach((item, i) => {
          code += item.getText() + "\n"
      });
      if (code !=  "") {
        this.sendhttp(code)
      }
    }
  },


  sendhttp(code) {
    const code_base64 = Buffer.from(code).toString('base64')

    const opts = {
      method: 'POST',
      url: config.url,
      body: {
        code: code_base64
      },
      json: true
    }
    get.concat(opts, function (err, res, data) {
      if (err) {
        atom.notifications.addError("Is soljit running? Is the URL in config.json correct?", {detail: err})
        return
      }
      if (!(typeof data.stdout_str === 'undefined')) {
        console.log(data.stdout_str)
      }
      if (!(typeof data.status === 'undefined')) {
        if (data.status == "lua") {
          atom.notifications.addWarning(data.what)
        } else if (data.status != "OK") {
          atom.notifications.addWarning(data.status)
        } else {
          atom.notifications.addSuccess(data.output)
        }
      }
    })
  },

  deactivate() {
    this.modalPanel.destroy();
    this.subscriptions.dispose();
    this.sendSelectionView.destroy();
  },

  serialize() {
    return {
      sendSelectionViewState: this.sendSelectionView.serialize()
    };
  },

};
