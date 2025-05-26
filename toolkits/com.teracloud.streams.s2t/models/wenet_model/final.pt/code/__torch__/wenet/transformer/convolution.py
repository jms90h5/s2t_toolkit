class ConvolutionModule(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  lorder : int
  use_layer_norm : bool
  pointwise_conv1 : __torch__.torch.nn.modules.conv.Conv1d
  depthwise_conv : __torch__.torch.nn.modules.conv.___torch_mangle_2.Conv1d
  norm : __torch__.torch.nn.modules.normalization.LayerNorm
  pointwise_conv2 : __torch__.torch.nn.modules.conv.___torch_mangle_3.Conv1d
  activation : __torch__.torch.nn.modules.activation.SiLU
  def forward(self: __torch__.wenet.transformer.convolution.ConvolutionModule,
    x: Tensor,
    mask_pad: Tensor=CONSTANTS.c1,
    cache: Tensor=CONSTANTS.c3) -> Tuple[Tensor, Tensor]:
    x0 = torch.transpose(x, 1, 2)
    _0 = torch.gt(torch.size(mask_pad, 2), 0)
    if _0:
      _1 = torch.masked_fill_(x0, torch.bitwise_not(mask_pad), 0.)
    else:
      pass
    if torch.gt(self.lorder, 0):
      if torch.eq(torch.size(cache, 2), 0):
        x3 = __torch__.torch.nn.functional._pad(x0, [self.lorder, 0], "constant", 0., )
        x2 = x3
      else:
        _2 = torch.eq(torch.size(cache, 0), torch.size(x0, 0))
        if _2:
          pass
        else:
          ops.prim.RaiseException("AssertionError: ")
        _3 = torch.eq(torch.size(cache, 1), torch.size(x0, 1))
        if _3:
          pass
        else:
          ops.prim.RaiseException("AssertionError: ")
        x2 = torch.cat([cache, x0], 2)
      _4 = torch.gt(torch.size(x2, 2), self.lorder)
      if _4:
        pass
      else:
        ops.prim.RaiseException("AssertionError: ")
      new_cache0 = torch.slice(torch.slice(torch.slice(x2), 1), 2, torch.neg(self.lorder))
      x1, new_cache = x2, new_cache0
    else:
      _5 = ops.prim.dtype(x0)
      _6 = ops.prim.device(x0)
      new_cache1 = torch.zeros([0, 0, 0], dtype=_5, layout=None, device=_6)
      x1, new_cache = x0, new_cache1
    x4 = (self.pointwise_conv1).forward(x1, )
    x5 = __torch__.torch.nn.functional.glu(x4, 1, )
    x6 = (self.depthwise_conv).forward(x5, )
    if self.use_layer_norm:
      x7 = torch.transpose(x6, 1, 2)
    else:
      x7 = x6
    x8 = (self.activation).forward((self.norm).forward(x7, ), )
    if self.use_layer_norm:
      x9 = torch.transpose(x8, 1, 2)
    else:
      x9 = x8
    x10 = (self.pointwise_conv2).forward(x9, )
    _7 = torch.gt(torch.size(mask_pad, 2), 0)
    if _7:
      _8 = torch.masked_fill_(x10, torch.bitwise_not(mask_pad), 0.)
    else:
      pass
    _9 = (torch.transpose(x10, 1, 2), new_cache)
    return _9
