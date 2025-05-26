def log_softmax(input: Tensor,
    dim: Optional[int]=None,
    _stacklevel: int=3,
    dtype: Optional[int]=None) -> Tensor:
  _0 = __torch__.torch.nn.functional._get_softmax_dim
  if torch.__is__(dim, None):
    dim1 = _0("log_softmax", torch.dim(input), _stacklevel, )
    dim0 = dim1
  else:
    dim0 = unchecked_cast(int, dim)
  if torch.__is__(dtype, None):
    ret = torch.log_softmax(input, dim0)
  else:
    dtype0 = unchecked_cast(int, dtype)
    ret0 = torch.log_softmax(input, dim0, dtype0)
    ret = ret0
  return ret
def relu(input: Tensor,
    inplace: bool=False) -> Tensor:
  if inplace:
    result = torch.relu_(input)
  else:
    result = torch.relu(input)
  return result
def dropout(input: Tensor,
    p: float=0.5,
    training: bool=True,
    inplace: bool=False) -> Tensor:
  _1 = "dropout probability has to be between 0 and 1, but got {}"
  if torch.lt(p, 0.):
    _2 = True
  else:
    _2 = torch.gt(p, 1.)
  if _2:
    ops.prim.RaiseException(torch.format(_1, p))
  else:
    pass
  if inplace:
    _3 = torch.dropout_(input, p, training)
  else:
    _3 = torch.dropout(input, p, training)
  return _3
def layer_norm(input: Tensor,
    normalized_shape: List[int],
    weight: Optional[Tensor]=None,
    bias: Optional[Tensor]=None,
    eps: float=1.0000000000000001e-05) -> Tensor:
  _4 = torch.layer_norm(input, normalized_shape, weight, bias, eps)
  return _4
def silu(input: Tensor,
    inplace: bool=False) -> Tensor:
  if inplace:
    _5 = torch.silu_(input)
  else:
    _5 = torch.silu(input)
  return _5
def _pad(input: Tensor,
    pad: List[int],
    mode: str="constant",
    value: float=0.) -> Tensor:
  _6 = "AssertionError: Padding length must be divisible by 2"
  _7 = "AssertionError: Padding length too large"
  _8 = "Padding mode \"{}\"\" doesn\'t take in value argument"
  _9 = "AssertionError: 3D tensors expect 2 values for padding"
  _10 = __torch__.torch.nn.functional._pad_circular
  _11 = "AssertionError: 4D tensors expect 4 values for padding"
  _12 = "AssertionError: 5D tensors expect 6 values for padding"
  _13 = "Only 3D, 4D, 5D padding with non-constant padding are supported for now"
  _14 = uninitialized(Tensor)
  _15 = torch.eq(torch.remainder(torch.len(pad), 2), 0)
  if _15:
    pass
  else:
    ops.prim.RaiseException(_6)
  _16 = torch.le(torch.floordiv(torch.len(pad), 2), torch.dim(input))
  if _16:
    pass
  else:
    ops.prim.RaiseException(_7)
  if torch.eq(mode, "constant"):
    _18 = torch.constant_pad_nd(input, pad, value)
    _17 = _18
  else:
    if torch.eq(value, 0):
      pass
    else:
      _19 = torch.add("AssertionError: ", torch.format(_8, mode))
      ops.prim.RaiseException(_19)
    if torch.eq(torch.dim(input), 3):
      if torch.eq(torch.len(pad), 2):
        pass
      else:
        ops.prim.RaiseException(_9)
      if torch.eq(mode, "reflect"):
        _22 = torch.reflection_pad1d(input, pad)
        _21 = _22
      else:
        if torch.eq(mode, "replicate"):
          _24 = torch.replication_pad1d(input, pad)
          _23 = _24
        else:
          if torch.eq(mode, "circular"):
            _25 = _10(input, pad, )
          else:
            ops.prim.RaiseException("")
            _25 = _14
          _23 = _25
        _21 = _23
      _20 = _21
    else:
      if torch.eq(torch.dim(input), 4):
        if torch.eq(torch.len(pad), 4):
          pass
        else:
          ops.prim.RaiseException(_11)
        if torch.eq(mode, "reflect"):
          _28 = torch.reflection_pad2d(input, pad)
          _27 = _28
        else:
          if torch.eq(mode, "replicate"):
            _30 = torch.replication_pad2d(input, pad)
            _29 = _30
          else:
            if torch.eq(mode, "circular"):
              _31 = _10(input, pad, )
            else:
              ops.prim.RaiseException("")
              _31 = _14
            _29 = _31
          _27 = _29
        _26 = _27
      else:
        if torch.eq(torch.dim(input), 5):
          if torch.eq(torch.len(pad), 6):
            pass
          else:
            ops.prim.RaiseException(_12)
          if torch.eq(mode, "reflect"):
            ops.prim.RaiseException("")
            _33 = _14
          else:
            if torch.eq(mode, "replicate"):
              _35 = torch.replication_pad3d(input, pad)
              _34 = _35
            else:
              _36 = torch.eq(mode, "circular")
              if _36:
                _37 = _10(input, pad, )
              else:
                ops.prim.RaiseException("")
                _37 = _14
              _34 = _37
            _33 = _34
          _32 = _33
        else:
          ops.prim.RaiseException(_13)
          _32 = _14
        _26 = _32
      _20 = _26
    _17 = _20
  return _17
def glu(input: Tensor,
    dim: int=-1) -> Tensor:
  _38 = "glu does not support scalars because halving size must be even"
  if torch.eq(torch.dim(input), 0):
    ops.prim.RaiseException(_38)
  else:
    pass
  return torch.glu(input, dim)
def embedding(input: Tensor,
    weight: Tensor,
    padding_idx: Optional[int]=None,
    max_norm: Optional[float]=None,
    norm_type: float=2.,
    scale_grad_by_freq: bool=False,
    sparse: bool=False) -> Tensor:
  _39 = "AssertionError: Padding_idx must be within num_embeddings"
  if torch.__isnot__(padding_idx, None):
    padding_idx1 = unchecked_cast(int, padding_idx)
    if torch.gt(padding_idx1, 0):
      _40 = torch.lt(padding_idx1, torch.size(weight, 0))
      if _40:
        pass
      else:
        ops.prim.RaiseException(_39)
      padding_idx2 = padding_idx1
    else:
      if torch.lt(padding_idx1, 0):
        _41 = torch.neg(torch.size(weight, 0))
        if torch.ge(padding_idx1, _41):
          pass
        else:
          ops.prim.RaiseException(_39)
        padding_idx4 = torch.add(torch.size(weight, 0), padding_idx1)
        padding_idx3 = padding_idx4
      else:
        padding_idx3 = padding_idx1
      padding_idx2 = padding_idx3
    padding_idx0 = padding_idx2
  else:
    padding_idx0 = -1
  if torch.__isnot__(max_norm, None):
    input0 = torch.contiguous(input)
  else:
    input0 = input
  _42 = torch.embedding(weight, input0, padding_idx0, scale_grad_by_freq, sparse)
  return _42
def ctc_loss(log_probs: Tensor,
    targets: Tensor,
    input_lengths: Tensor,
    target_lengths: Tensor,
    blank: int=0,
    reduction: str="mean",
    zero_infinity: bool=False) -> Tensor:
  _43 = __torch__.torch.nn._reduction.get_enum
  _44 = torch.ctc_loss(log_probs, targets, input_lengths, target_lengths, blank, _43(reduction, ), zero_infinity)
  return _44
def kl_div(input: Tensor,
    target: Tensor,
    size_average: Optional[bool]=None,
    reduce: Optional[bool]=None,
    reduction: str="mean",
    log_target: bool=False) -> Tensor:
  _45 = __torch__.torch.nn._reduction.legacy_get_enum
  _46 = "reduction: \'mean\' divides the total loss by both the batch size and the support size.\'batchmean\' divides only by the batch size, and aligns with the KL div math definition.\'mean\' will be changed to behave the same as \'batchmean\' in the next major release."
  _47 = __torch__.torch.nn._reduction.get_enum
  if torch.__isnot__(size_average, None):
    _48, size_average0 = True, unchecked_cast(bool, size_average)
  else:
    _48, size_average0 = torch.__isnot__(reduce, None), size_average
  if _48:
    reduction_enum = _45(size_average0, reduce, True, )
  else:
    if torch.eq(reduction, "mean"):
      torch.warn(_46)
    else:
      pass
    if torch.eq(reduction, "batchmean"):
      reduction_enum0 = _47("sum", )
    else:
      reduction_enum0 = _47(reduction, )
    reduction_enum = reduction_enum0
  reduced = torch.kl_div(input, target, reduction_enum, log_target=log_target)
  if torch.eq(reduction, "batchmean"):
    _49 = torch.ne(torch.dim(input), 0)
  else:
    _49 = False
  if _49:
    reduced1 = torch.div(reduced, (torch.size(input))[0])
    reduced0 = reduced1
  else:
    reduced0 = reduced
  return reduced0
def _get_softmax_dim(name: str,
    ndim: int,
    stacklevel: int) -> int:
  _50 = "Implicit dimension choice for {} has been deprecated. Change the call to include dim=X as an argument."
  torch.warn(torch.format(_50, name), stacklevel)
  if torch.eq(ndim, 0):
    _51 = True
  else:
    _51 = torch.eq(ndim, 1)
  if _51:
    _52 = True
  else:
    _52 = torch.eq(ndim, 3)
  if _52:
    ret = 0
  else:
    ret = 1
  return ret
def _pad_circular(input: Tensor,
    padding: List[int]) -> Tensor:
  _53 = "AssertionError: Padding value causes wrapping around more than once."
  _54 = "AssertionError: Negative padding value is resulting in an empty dimension."
  in_shape = torch.size(input)
  paddable_shape = torch.slice(in_shape, 2)
  ndim = torch.len(paddable_shape)
  _55 = [9223372036854775807, torch.len(paddable_shape)]
  for idx in range(ops.prim.min(_55)):
    size = paddable_shape[idx]
    _56 = torch.neg(torch.add(torch.mul(idx, 2), 1))
    if torch.le(padding[_56], size):
      pass
    else:
      ops.prim.RaiseException(_53)
    _57 = torch.neg(torch.add(torch.mul(idx, 2), 2))
    if torch.le(padding[_57], size):
      pass
    else:
      ops.prim.RaiseException(_53)
    _58 = torch.neg(torch.add(torch.mul(idx, 2), 1))
    _59 = padding[_58]
    _60 = torch.neg(torch.add(torch.mul(idx, 2), 2))
    _61 = torch.add(torch.add(_59, padding[_60]), size)
    if torch.ge(_61, 0):
      pass
    else:
      ops.prim.RaiseException(_54)
  out_shape = torch.slice(in_shape, None, 2)
  _62 = [9223372036854775807, torch.len(paddable_shape)]
  out_shape0 = out_shape
  for idx0 in range(ops.prim.min(_62)):
    size0 = paddable_shape[idx0]
    _63 = torch.neg(torch.add(torch.mul(idx0, 2), 1))
    _64 = torch.add(size0, padding[_63])
    _65 = torch.neg(torch.add(torch.mul(idx0, 2), 2))
    out_shape1 = torch.add_(out_shape0, [torch.add(_64, padding[_65])])
    out_shape0 = out_shape1
  out = torch.empty(out_shape0, dtype=ops.prim.dtype(input), layout=ops.prim.layout(input), device=ops.prim.device(input))
  if torch.eq(ndim, 1):
    out_d0 = ops.prim.max(padding[-2], 0)
    out_d1 = torch.sub(out_shape0[2], ops.prim.max(padding[-1], 0))
    in_d0 = ops.prim.max(torch.neg(padding[-2]), 0)
    _66 = in_shape[2]
    _67 = ops.prim.max(torch.neg(padding[-1]), 0)
    in_d1 = torch.sub(_66, _67)
    _68 = torch.slice(input, -1, in_d0, in_d1)
    _69 = torch.slice(out, -1, out_d0, out_d1)
    _70 = torch.copy_(_69, _68)
  else:
    if torch.eq(ndim, 2):
      out_d00 = ops.prim.max(padding[-2], 0)
      out_d10 = torch.sub(out_shape0[2], ops.prim.max(padding[-1], 0))
      out_h0 = ops.prim.max(padding[-4], 0)
      out_h1 = torch.sub(out_shape0[3], ops.prim.max(padding[-3], 0))
      in_d00 = ops.prim.max(torch.neg(padding[-2]), 0)
      _71 = in_shape[2]
      _72 = ops.prim.max(torch.neg(padding[-1]), 0)
      in_d10 = torch.sub(_71, _72)
      in_h0 = ops.prim.max(torch.neg(padding[-4]), 0)
      _73 = in_shape[3]
      _74 = ops.prim.max(torch.neg(padding[-3]), 0)
      in_h1 = torch.sub(_73, _74)
      _75 = torch.slice(input, -2, in_d00, in_d10)
      _76 = torch.slice(_75, -1, in_h0, in_h1)
      _77 = torch.slice(out, -2, out_d00, out_d10)
      _78 = torch.slice(_77, -1, out_h0, out_h1)
      _79 = torch.copy_(_78, _76)
    else:
      if torch.eq(ndim, 3):
        out_d01 = ops.prim.max(padding[-2], 0)
        out_d11 = torch.sub(out_shape0[2], ops.prim.max(padding[-1], 0))
        out_h00 = ops.prim.max(padding[-4], 0)
        out_h10 = torch.sub(out_shape0[3], ops.prim.max(padding[-3], 0))
        out_w0 = ops.prim.max(padding[-6], 0)
        out_w1 = torch.sub(out_shape0[4], ops.prim.max(padding[-5], 0))
        in_d01 = ops.prim.max(torch.neg(padding[-2]), 0)
        _80 = in_shape[2]
        _81 = ops.prim.max(torch.neg(padding[-1]), 0)
        in_d11 = torch.sub(_80, _81)
        in_h00 = ops.prim.max(torch.neg(padding[-4]), 0)
        _82 = in_shape[3]
        _83 = ops.prim.max(torch.neg(padding[-3]), 0)
        in_h10 = torch.sub(_82, _83)
        in_w0 = ops.prim.max(torch.neg(padding[-6]), 0)
        _84 = in_shape[4]
        _85 = ops.prim.max(torch.neg(padding[-5]), 0)
        in_w1 = torch.sub(_84, _85)
        _86 = torch.slice(input, -3, in_d01, in_d11)
        _87 = torch.slice(_86, -2, in_h00, in_h10)
        _88 = torch.slice(_87, -1, in_w0, in_w1)
        _89 = torch.slice(out, -3, out_d01, out_d11)
        _90 = torch.slice(_89, -2, out_h00, out_h10)
        _91 = torch.slice(_90, -1, out_w0, out_w1)
        _92 = torch.copy_(_91, _88)
      else:
        pass
  if torch.gt(padding[-2], 0):
    _93 = torch.sub(out_shape0[2], padding[-2])
    i0 = torch.sub(_93, ops.prim.max(padding[-1], 0))
    i1 = torch.sub(out_shape0[2], ops.prim.max(padding[-1], 0))
    o1 = padding[-2]
    _94 = torch.slice(torch.slice(torch.slice(out), 1), 2, i0, i1)
    _95 = torch.slice(torch.slice(torch.slice(out), 1), 2, 0, o1)
    _96 = torch.copy_(_95, _94)
  else:
    pass
  if torch.gt(padding[-1], 0):
    i00 = ops.prim.max(padding[-2], 0)
    i10 = torch.add(ops.prim.max(padding[-2], 0), padding[-1])
    o0 = torch.sub(out_shape0[2], padding[-1])
    o10 = out_shape0[2]
    _97 = torch.slice(torch.slice(torch.slice(out), 1), 2, i00, i10)
    _98 = torch.slice(torch.slice(torch.slice(out), 1), 2, o0, o10)
    _99 = torch.copy_(_98, _97)
  else:
    pass
  if torch.gt(torch.len(padding), 2):
    if torch.gt(padding[-4], 0):
      _100 = torch.sub(out_shape0[3], padding[-4])
      i01 = torch.sub(_100, ops.prim.max(padding[-3], 0))
      i11 = torch.sub(out_shape0[3], ops.prim.max(padding[-3], 0))
      o11 = padding[-4]
      _101 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _102 = torch.slice(_101, 3, i01, i11)
      _103 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _104 = torch.copy_(torch.slice(_103, 3, 0, o11), _102)
    else:
      pass
    if torch.gt(padding[-3], 0):
      i02 = ops.prim.max(padding[-4], 0)
      i12 = torch.add(ops.prim.max(padding[-4], 0), padding[-3])
      o00 = torch.sub(out_shape0[3], padding[-3])
      o12 = out_shape0[3]
      _105 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _106 = torch.slice(_105, 3, i02, i12)
      _107 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _108 = torch.copy_(torch.slice(_107, 3, o00, o12), _106)
    else:
      pass
  else:
    pass
  if torch.gt(torch.len(padding), 4):
    if torch.gt(padding[-6], 0):
      _109 = torch.sub(out_shape0[4], padding[-6])
      i03 = torch.sub(_109, ops.prim.max(padding[-5], 0))
      i13 = torch.sub(out_shape0[4], ops.prim.max(padding[-5], 0))
      o13 = padding[-6]
      _110 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _111 = torch.slice(torch.slice(_110, 3), 4, i03, i13)
      _112 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _113 = torch.slice(torch.slice(_112, 3), 4, 0, o13)
      _114 = torch.copy_(_113, _111)
    else:
      pass
    if torch.gt(padding[-5], 0):
      i04 = ops.prim.max(padding[-6], 0)
      i14 = torch.add(ops.prim.max(padding[-6], 0), padding[-5])
      o01 = torch.sub(out_shape0[4], padding[-5])
      o14 = out_shape0[4]
      _115 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _116 = torch.slice(torch.slice(_115, 3), 4, i04, i14)
      _117 = torch.slice(torch.slice(torch.slice(out), 1), 2)
      _118 = torch.slice(torch.slice(_117, 3), 4, o01, o14)
      _119 = torch.copy_(_118, _116)
    else:
      pass
  else:
    pass
  return out
