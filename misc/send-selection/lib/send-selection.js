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

    console.log("activated");
  },

  send_line() {
    const editor = atom.workspace.getActiveTextEditor()
    if (editor) {
      const p = editor.getCursorBufferPosition()
      const line = editor.lineTextForBufferRow(p.row)
      console.log(line);
      this.sendhttp(line)
    }

  },

  send_selection() {
    const editor = atom.workspace.getActiveTextEditor()
    if (editor) {
      const selection = editor.getSelectedBufferRanges()
      code = ""
      editor.getSelections().forEach((item, i) => {
          console.log(item.getText())
          code += item.getText() + "\n"
      });
      console.log(code);
      this.sendhttp(code)
    }
  },

  sendhttp(code) {
    console.log("As base64:");
    const code_base64 = Buffer.from(code).toString('base64')
    console.log(code_base64);

    const opts = {
      method: 'POST',
      url: config.url,
      body: {
        code: code_base64
      },
      json: true
    }
    get.concat(opts, function (err, res, data) {
      if (err) console.log("ERROR", err)
      console.log(data) // `data` is an object
    })
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
