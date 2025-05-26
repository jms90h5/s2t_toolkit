class ASRModel(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  sos : int
  eos : int
  vocab_size : int
  ignore_id : int
  ctc_weight : float
  reverse_weight : float
  encoder : __torch__.wenet.transformer.encoder.ConformerEncoder
  decoder : __torch__.wenet.transformer.decoder.BiTransformerDecoder
  ctc : __torch__.wenet.transformer.ctc.CTC
  criterion_att : __torch__.wenet.transformer.label_smoothing_loss.LabelSmoothingLoss
  def forward(self: __torch__.wenet.transformer.asr_model.ASRModel,
    speech: Tensor,
    speech_lengths: Tensor,
    text: Tensor,
    text_lengths: Tensor) -> Tuple[Optional[Tensor], Optional[Tensor], Optional[Tensor]]:
    _0 = torch.eq(torch.dim(text_lengths), 1)
    if _0:
      pass
    else:
      _1 = torch.add("AssertionError: ", str(torch.size(text_lengths)))
      ops.prim.RaiseException(_1)
    _2 = torch.eq((torch.size(speech))[0], (torch.size(speech_lengths))[0])
    if _2:
      _4 = torch.eq((torch.size(speech_lengths))[0], (torch.size(text))[0])
      _3 = _4
    else:
      _3 = False
    if _3:
      _6 = torch.eq((torch.size(text))[0], (torch.size(text_lengths))[0])
      _5 = _6
    else:
      _5 = False
    if _5:
      pass
    else:
      _7 = (torch.size(speech), torch.size(speech_lengths), torch.size(text), torch.size(text_lengths))
      _8 = torch.add("AssertionError: ", str(_7))
      ops.prim.RaiseException(_8)
    _9 = (self.encoder).forward(speech, speech_lengths, 0, -1, )
    encoder_out, encoder_mask, = _9
    encoder_out_lens = torch.sum(torch.squeeze(encoder_mask, 1), [1])
    if torch.ne(self.ctc_weight, 1.):
      _10 = (self)._calc_att_loss(encoder_out, encoder_mask, text, text_lengths, )
      loss_att0, acc_att, = _10
      loss_att = loss_att0
    else:
      loss_att = None
    if torch.ne(self.ctc_weight, 0.):
      loss_ctc0 = (self.ctc).forward(encoder_out, encoder_out_lens, text, text_lengths, )
      loss_ctc = loss_ctc0
    else:
      loss_ctc = None
    if torch.__is__(loss_ctc, None):
      loss, loss_att1, loss_ctc1 = loss_att, loss_att, loss_ctc
    else:
      loss_ctc2 = unchecked_cast(Tensor, loss_ctc)
      if torch.__is__(loss_att, None):
        loss0, loss_att2 = loss_ctc2, loss_att
      else:
        loss_att3 = unchecked_cast(Tensor, loss_att)
        _11 = torch.mul(loss_ctc2, self.ctc_weight)
        _12 = torch.mul(loss_att3, torch.sub(1, self.ctc_weight))
        loss0, loss_att2 = torch.add(_11, _12), loss_att3
      loss, loss_att1, loss_ctc1 = loss0, loss_att2, loss_ctc2
    return (loss, loss_att1, loss_ctc1)
  def ctc_activation(self: __torch__.wenet.transformer.asr_model.ASRModel,
    xs: Tensor) -> Tensor:
    return (self.ctc).log_softmax(xs, )
  def eos_symbol(self: __torch__.wenet.transformer.asr_model.ASRModel) -> int:
    return self.eos
  def forward_attention_decoder(self: __torch__.wenet.transformer.asr_model.ASRModel,
    hyps: Tensor,
    hyps_lens: Tensor,
    encoder_out: Tensor,
    reverse_weight: float=0.) -> Tuple[Tensor, Tensor]:
    _13 = __torch__.wenet.utils.common.reverse_pad_list
    _14 = __torch__.wenet.utils.common.add_sos_eos
    _15 = __torch__.torch.nn.functional.log_softmax
    _16 = torch.eq(torch.size(encoder_out, 0), 1)
    if _16:
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    num_hyps = torch.size(hyps, 0)
    _17 = torch.eq(torch.size(hyps_lens, 0), num_hyps)
    if _17:
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    encoder_out0 = torch.repeat(encoder_out, [num_hyps, 1, 1])
    _18 = torch.size(encoder_out0, 1)
    _19 = ops.prim.device(encoder_out0)
    encoder_mask = torch.ones([num_hyps, 1, _18], dtype=11, layout=None, device=_19)
    r_hyps_lens = torch.sub(hyps_lens, 1)
    r_hyps = torch.slice(torch.slice(hyps), 1, 1)
    r_hyps0 = _13(r_hyps, r_hyps_lens, float(self.ignore_id), )
    _20 = _14(r_hyps0, self.sos, self.eos, self.ignore_id, )
    r_hyps1, _21, = _20
    _22 = (self.decoder).forward(encoder_out0, encoder_mask, hyps, hyps_lens, r_hyps1, reverse_weight, )
    decoder_out, r_decoder_out, _23, = _22
    decoder_out0 = _15(decoder_out, -1, 3, None, )
    r_decoder_out0 = _15(r_decoder_out, -1, 3, None, )
    return (decoder_out0, r_decoder_out0)
  def forward_encoder_chunk(self: __torch__.wenet.transformer.asr_model.ASRModel,
    xs: Tensor,
    offset: int,
    required_cache_size: int,
    att_cache: Tensor=CONSTANTS.c0,
    cnn_cache: Tensor=CONSTANTS.c0) -> Tuple[Tensor, Tensor, Tensor]:
    _24 = (self.encoder).forward_chunk(xs, offset, required_cache_size, att_cache, cnn_cache, )
    return _24
  def is_bidirectional_decoder(self: __torch__.wenet.transformer.asr_model.ASRModel) -> bool:
    return True
  def right_context(self: __torch__.wenet.transformer.asr_model.ASRModel) -> int:
    return self.encoder.embed.right_context
  def sos_symbol(self: __torch__.wenet.transformer.asr_model.ASRModel) -> int:
    return self.sos
  def subsampling_rate(self: __torch__.wenet.transformer.asr_model.ASRModel) -> int:
    return self.encoder.embed.subsampling_rate
  def _calc_att_loss(self: __torch__.wenet.transformer.asr_model.ASRModel,
    encoder_out: Tensor,
    encoder_mask: Tensor,
    ys_pad: Tensor,
    ys_pad_lens: Tensor) -> Tuple[Tensor, float]:
    _25 = __torch__.wenet.utils.common.add_sos_eos
    _26 = __torch__.wenet.utils.common.reverse_pad_list
    _27 = __torch__.wenet.utils.common.th_accuracy
    _28 = _25(ys_pad, self.sos, self.eos, self.ignore_id, )
    ys_in_pad, ys_out_pad, = _28
    ys_in_lens = torch.add(ys_pad_lens, 1)
    r_ys_pad = _26(ys_pad, ys_pad_lens, float(self.ignore_id), )
    _29 = _25(r_ys_pad, self.sos, self.eos, self.ignore_id, )
    r_ys_in_pad, r_ys_out_pad, = _29
    _30 = (self.decoder).forward(encoder_out, encoder_mask, ys_in_pad, ys_in_lens, r_ys_in_pad, self.reverse_weight, )
    decoder_out, r_decoder_out, _31, = _30
    loss_att = (self.criterion_att).forward(decoder_out, ys_out_pad, )
    r_loss_att = torch.tensor(0.)
    if torch.gt(self.reverse_weight, 0.):
      r_loss_att1 = (self.criterion_att).forward(r_decoder_out, r_ys_out_pad, )
      r_loss_att0 = r_loss_att1
    else:
      r_loss_att0 = r_loss_att
    _32 = torch.mul(loss_att, torch.sub(1, self.reverse_weight))
    _33 = torch.mul(r_loss_att0, self.reverse_weight)
    loss_att4 = torch.add(_32, _33)
    _34 = torch.view(decoder_out, [-1, self.vocab_size])
    acc_att = _27(_34, ys_out_pad, self.ignore_id, )
    return (loss_att4, acc_att)
