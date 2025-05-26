class BiTransformerDecoder(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  left_decoder : __torch__.wenet.transformer.decoder.TransformerDecoder
  right_decoder : __torch__.wenet.transformer.decoder.TransformerDecoder
  def forward(self: __torch__.wenet.transformer.decoder.BiTransformerDecoder,
    memory: Tensor,
    memory_mask: Tensor,
    ys_in_pad: Tensor,
    ys_in_lens: Tensor,
    r_ys_in_pad: Tensor,
    reverse_weight: float=0.) -> Tuple[Tensor, Tensor, Tensor]:
    _0 = (self.left_decoder).forward(memory, memory_mask, ys_in_pad, ys_in_lens, None, 0., )
    l_x, _1, olens, = _0
    r_x = torch.tensor(0.)
    if torch.gt(reverse_weight, 0.):
      _2 = (self.right_decoder).forward(memory, memory_mask, r_ys_in_pad, ys_in_lens, None, 0., )
      r_x1, _3, olens1, = _2
      r_x0, olens0 = r_x1, olens1
    else:
      r_x0, olens0 = r_x, olens
    return (l_x, r_x0, olens0)
class TransformerDecoder(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  normalize_before : bool
  use_output_layer : bool
  num_blocks : int
  embed : __torch__.torch.nn.modules.container.___torch_mangle_4.Sequential
  after_norm : __torch__.torch.nn.modules.normalization.LayerNorm
  output_layer : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  decoders : __torch__.torch.nn.modules.container.___torch_mangle_6.ModuleList
  def forward(self: __torch__.wenet.transformer.decoder.TransformerDecoder,
    memory: Tensor,
    memory_mask: Tensor,
    ys_in_pad: Tensor,
    ys_in_lens: Tensor,
    r_ys_in_pad: Optional[Tensor]=None,
    reverse_weight: float=0.) -> Tuple[Tensor, Tensor, Tensor]:
    _4 = __torch__.wenet.utils.mask.make_pad_mask
    _5 = __torch__.wenet.utils.mask.subsequent_mask
    maxlen = torch.size(ys_in_pad, 1)
    _6 = torch.unsqueeze(_4(ys_in_lens, maxlen, ), 1)
    tgt_mask = torch.bitwise_not(_6)
    tgt_mask0 = torch.to(tgt_mask, ops.prim.device(ys_in_pad))
    _7 = _5(torch.size(tgt_mask0, -1), ops.prim.device(tgt_mask0), )
    m = torch.unsqueeze(_7, 0)
    tgt_mask1 = torch.__and__(tgt_mask0, m)
    x, _8, = (self.embed).forward(ys_in_pad, )
    _9 = self.decoders
    _10 = getattr(_9, "0")
    _11 = getattr(_9, "1")
    _12 = getattr(_9, "2")
    _13 = (_10).forward(x, tgt_mask1, memory, memory_mask, None, )
    x0, tgt_mask2, memory0, memory_mask0, = _13
    _14 = (_11).forward(x0, tgt_mask2, memory0, memory_mask0, None, )
    x1, tgt_mask3, memory1, memory_mask1, = _14
    _15 = (_12).forward(x1, tgt_mask3, memory1, memory_mask1, None, )
    x2, tgt_mask4, memory2, memory_mask2, = _15
    if self.normalize_before:
      x3 = (self.after_norm).forward(x2, )
    else:
      x3 = x2
    if self.use_output_layer:
      x4 = (self.output_layer).forward(x3, )
    else:
      x4 = x3
    olens = torch.sum(tgt_mask4, [1])
    return (x4, torch.tensor(0.), olens)
