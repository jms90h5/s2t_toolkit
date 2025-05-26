class Linear(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  in_features : int
  out_features : int
  scale : float
  zero_point : int
  version : int
  _packed_params : __torch__.torch.nn.quantized.modules.linear.LinearPackedParams
  def forward(self: __torch__.torch.nn.quantized.dynamic.modules.linear.Linear,
    x: Tensor) -> Tensor:
    _0 = "Unsupported dtype on dynamic quantized linear!"
    _1 = uninitialized(Tensor)
    _2 = torch.eq(self._packed_params.dtype, 12)
    if _2:
      if torch.lt(self.version, 4):
        _3 = self._packed_params._packed_params
        Y1 = ops.quantized.linear_dynamic(x, _3)
        Y0 = Y1
      else:
        _4 = self._packed_params._packed_params
        Y2 = ops.quantized.linear_dynamic(x, _4, True)
        Y0 = Y2
      Y = Y0
    else:
      _5 = torch.eq(self._packed_params.dtype, 5)
      if _5:
        _6 = self._packed_params._packed_params
        Y4 = ops.quantized.linear_dynamic_fp16(x, _6)
        Y3 = Y4
      else:
        ops.prim.RaiseException(_0)
        Y3 = _1
      Y = Y3
    return torch.to(Y, ops.prim.dtype(x))
