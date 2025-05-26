class ConformerEncoderLayer(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  ff_scale : float
  size : int
  normalize_before : bool
  concat_after : bool
  self_attn : __torch__.wenet.transformer.attention.RelPositionMultiHeadedAttention
  feed_forward : __torch__.wenet.transformer.positionwise_feed_forward.PositionwiseFeedForward
  feed_forward_macaron : __torch__.wenet.transformer.positionwise_feed_forward.PositionwiseFeedForward
  conv_module : __torch__.wenet.transformer.convolution.ConvolutionModule
  norm_ff : __torch__.torch.nn.modules.normalization.LayerNorm
  norm_mha : __torch__.torch.nn.modules.normalization.LayerNorm
  norm_ff_macaron : __torch__.torch.nn.modules.normalization.LayerNorm
  norm_conv : __torch__.torch.nn.modules.normalization.LayerNorm
  norm_final : __torch__.torch.nn.modules.normalization.LayerNorm
  dropout : __torch__.torch.nn.modules.dropout.Dropout
  concat_linear : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  def forward(self: __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer,
    x: Tensor,
    mask: Tensor,
    pos_emb: Tensor,
    mask_pad: Tensor=CONSTANTS.c1,
    att_cache: Tensor=CONSTANTS.c0,
    cnn_cache: Tensor=CONSTANTS.c0) -> Tuple[Tensor, Tensor, Tensor, Tensor]:
    if self.normalize_before:
      x1 = (self.norm_ff_macaron).forward(x, )
      x0 = x1
    else:
      x0 = x
    _0 = self.ff_scale
    _1 = self.dropout
    _2 = (self.feed_forward_macaron).forward(x0, )
    x2 = torch.add(x, torch.mul((_1).forward(_2, ), _0))
    _3 = torch.__not__(self.normalize_before)
    if _3:
      x4 = (self.norm_ff_macaron).forward(x2, )
      x3 = x4
    else:
      x3 = x2
    if self.normalize_before:
      x5 = (self.norm_mha).forward(x3, )
    else:
      x5 = x3
    _4 = (self.self_attn).forward(x5, x5, x5, mask, pos_emb, att_cache, )
    x_att, new_att_cache, = _4
    if self.concat_after:
      x_concat = torch.cat([x5, x_att], -1)
      _5 = (self.concat_linear).forward(x_concat, )
      x6 = torch.add(x3, _5)
    else:
      x7 = torch.add(x3, (self.dropout).forward(x_att, ))
      x6 = x7
    _6 = torch.__not__(self.normalize_before)
    if _6:
      x8 = (self.norm_mha).forward(x6, )
    else:
      x8 = x6
    if self.normalize_before:
      x9 = (self.norm_conv).forward(x8, )
    else:
      x9 = x8
    _7 = (self.conv_module).forward(x9, mask_pad, cnn_cache, )
    x10, new_cnn_cache, = _7
    x11 = torch.add(x8, (self.dropout).forward(x10, ))
    _8 = torch.__not__(self.normalize_before)
    if _8:
      x12 = (self.norm_conv).forward(x11, )
    else:
      x12 = x11
    if self.normalize_before:
      x13 = (self.norm_ff).forward(x12, )
    else:
      x13 = x12
    _9 = self.ff_scale
    _10 = (self.dropout).forward((self.feed_forward).forward(x13, ), )
    x14 = torch.add(x12, torch.mul(_10, _9))
    _11 = torch.__not__(self.normalize_before)
    if _11:
      x15 = (self.norm_ff).forward(x14, )
    else:
      x15 = x14
    x16 = (self.norm_final).forward(x15, )
    _12 = (x16, mask, new_att_cache, new_cnn_cache)
    return _12
