def get_enum(reduction: str) -> int:
  _0 = "reduction=\'elementwise_mean\' is deprecated, please use reduction=\'mean\' instead."
  _1 = "{} is not a valid value for reduction"
  _2 = uninitialized(int)
  if torch.eq(reduction, "none"):
    ret = 0
  else:
    if torch.eq(reduction, "mean"):
      ret0 = 1
    else:
      _3 = torch.eq(reduction, "elementwise_mean")
      if _3:
        torch.warn(_0)
        ret1 = 1
      else:
        if torch.eq(reduction, "sum"):
          ret2 = 2
        else:
          ops.prim.RaiseException(torch.format(_1, reduction))
          ret2 = _2
        ret1 = ret2
      ret0 = ret1
    ret = ret0
  return ret
def legacy_get_enum(size_average: Optional[bool],
    reduce: Optional[bool],
    emit_warning: bool=True) -> int:
  _4 = __torch__.torch.nn._reduction.legacy_get_string
  _5 = __torch__.torch.nn._reduction.get_enum
  _6 = _4(size_average, reduce, emit_warning, )
  return _5(_6, )
def legacy_get_string(size_average: Optional[bool],
    reduce: Optional[bool],
    emit_warning: bool=True) -> str:
  warning = "size_average and reduce args will be deprecated, please use reduction=\'{}\' instead."
  if torch.__is__(size_average, None):
    size_average0 = True
  else:
    size_average0 = unchecked_cast(bool, size_average)
  if torch.__is__(reduce, None):
    reduce0 = True
  else:
    reduce0 = unchecked_cast(bool, reduce)
  if size_average0:
    _7 = reduce0
  else:
    _7 = False
  if _7:
    ret = "mean"
  else:
    if reduce0:
      ret3 = "sum"
    else:
      ret3 = "none"
    ret = ret3
  if emit_warning:
    torch.warn(torch.format(warning, ret))
  else:
    pass
  return ret
