class DecoderLayer(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  size : int
  normalize_before : bool
  concat_after : bool
  self_attn : __torch__.wenet.transformer.attention.MultiHeadedAttention
  src_attn : __torch__.wenet.transformer.attention.MultiHeadedAttention
  feed_forward : __torch__.wenet.transformer.positionwise_feed_forward.___torch_mangle_5.PositionwiseFeedForward
  norm1 : __torch__.torch.nn.modules.normalization.LayerNorm
  norm2 : __torch__.torch.nn.modules.normalization.LayerNorm
  norm3 : __torch__.torch.nn.modules.normalization.LayerNorm
  dropout : __torch__.torch.nn.modules.dropout.Dropout
  concat_linear1 : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  concat_linear2 : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  def forward(self: __torch__.wenet.transformer.decoder_layer.DecoderLayer,
    tgt: Tensor,
    tgt_mask: Tensor,
    memory: Tensor,
    memory_mask: Tensor,
    cache: Optional[Tensor]=None) -> Tuple[Tensor, Tensor, Tensor, Tensor]:
    _0 = "AssertionError: {cache.shape} == {(tgt.shape[0], tgt.shape[1] - 1, self.size)}"
    if self.normalize_before:
      tgt0 = (self.norm1).forward(tgt, )
    else:
      tgt0 = tgt
    if torch.__is__(cache, None):
      tgt_q, tgt_q_mask, residual, cache0 = tgt0, tgt_mask, tgt, cache
    else:
      cache1 = unchecked_cast(Tensor, cache)
      _1 = torch.size(cache1)
      _2 = (torch.size(tgt0))[0]
      _3 = torch.sub((torch.size(tgt0))[1], 1)
      if torch.eq(_1, [_2, _3, self.size]):
        pass
      else:
        ops.prim.RaiseException(_0)
      _4 = torch.slice(torch.slice(tgt0), 1, -1)
      tgt_q0 = torch.slice(_4, 2)
      _5 = torch.slice(torch.slice(tgt), 1, -1)
      residual0 = torch.slice(_5, 2)
      _6 = torch.slice(torch.slice(tgt_mask), 1, -1)
      tgt_q, tgt_q_mask, residual, cache0 = tgt_q0, torch.slice(_6, 2), residual0, cache1
    if self.concat_after:
      _7 = (self.self_attn).forward(tgt_q, tgt0, tgt0, tgt_q_mask, CONSTANTS.c2, CONSTANTS.c0, )
      tgt_concat = torch.cat([tgt_q, (_7)[0]], -1)
      _8 = (self.concat_linear1).forward(tgt_concat, )
      x = torch.add(residual, _8)
    else:
      _9 = self.dropout
      _10 = (self.self_attn).forward(tgt_q, tgt0, tgt0, tgt_q_mask, CONSTANTS.c2, CONSTANTS.c0, )
      x0 = torch.add(residual, (_9).forward((_10)[0], ))
      x = x0
    _11 = torch.__not__(self.normalize_before)
    if _11:
      x1 = (self.norm1).forward(x, )
    else:
      x1 = x
    if self.normalize_before:
      x2 = (self.norm2).forward(x1, )
    else:
      x2 = x1
    if self.concat_after:
      _12 = (self.src_attn).forward(x2, memory, memory, memory_mask, CONSTANTS.c2, CONSTANTS.c0, )
      x_concat = torch.cat([x2, (_12)[0]], -1)
      _13 = (self.concat_linear2).forward(x_concat, )
      x3 = torch.add(x1, _13)
    else:
      _14 = self.dropout
      _15 = (self.src_attn).forward(x2, memory, memory, memory_mask, CONSTANTS.c2, CONSTANTS.c0, )
      x4 = torch.add(x1, (_14).forward((_15)[0], ))
      x3 = x4
    _16 = torch.__not__(self.normalize_before)
    if _16:
      x5 = (self.norm2).forward(x3, )
    else:
      x5 = x3
    if self.normalize_before:
      x6 = (self.norm3).forward(x5, )
    else:
      x6 = x5
    _17 = (self.dropout).forward((self.feed_forward).forward(x6, ), )
    x7 = torch.add(x5, _17)
    _18 = torch.__not__(self.normalize_before)
    if _18:
      x8 = (self.norm3).forward(x7, )
    else:
      x8 = x7
    if torch.__isnot__(cache0, None):
      cache2 = unchecked_cast(Tensor, cache0)
      x9 = torch.cat([cache2, x8], 1)
    else:
      x9 = x8
    return (x9, tgt_mask, memory, memory_mask)
