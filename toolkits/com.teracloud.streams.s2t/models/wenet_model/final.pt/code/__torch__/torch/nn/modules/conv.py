class Conv2d(Module):
  __parameters__ = ["weight", "bias", ]
  __buffers__ = []
  weight : Tensor
  bias : Optional[Tensor]
  training : bool
  _is_full_backward_hook : NoneType
  transposed : bool
  _reversed_padding_repeated_twice : List[int]
  out_channels : Final[int] = 512
  kernel_size : Final[Tuple[int, int]] = (3, 3)
  in_channels : Final[int] = 1
  output_padding : Final[Tuple[int, int]] = (0, 0)
  dilation : Final[Tuple[int, int]] = (1, 1)
  stride : Final[Tuple[int, int]] = (2, 2)
  padding : Final[Tuple[int, int]] = (0, 0)
  groups : Final[int] = 1
  padding_mode : Final[str] = "zeros"
  def forward(self: __torch__.torch.nn.modules.conv.Conv2d,
    input: Tensor) -> Tensor:
    _0 = (self)._conv_forward(input, self.weight, self.bias, )
    return _0
  def _conv_forward(self: __torch__.torch.nn.modules.conv.Conv2d,
    input: Tensor,
    weight: Tensor,
    bias: Optional[Tensor]) -> Tensor:
    _1 = torch.conv2d(input, weight, bias, [2, 2], [0, 0], [1, 1])
    return _1
class Conv1d(Module):
  __parameters__ = ["weight", "bias", ]
  __buffers__ = []
  weight : Tensor
  bias : Optional[Tensor]
  training : bool
  _is_full_backward_hook : NoneType
  transposed : bool
  _reversed_padding_repeated_twice : List[int]
  out_channels : Final[int] = 1024
  kernel_size : Final[Tuple[int]] = (1,)
  in_channels : Final[int] = 512
  output_padding : Final[Tuple[int]] = (0,)
  dilation : Final[Tuple[int]] = (1,)
  stride : Final[Tuple[int]] = (1,)
  padding : Final[Tuple[int]] = (0,)
  groups : Final[int] = 1
  padding_mode : Final[str] = "zeros"
  def forward(self: __torch__.torch.nn.modules.conv.Conv1d,
    input: Tensor) -> Tensor:
    _2 = (self)._conv_forward(input, self.weight, self.bias, )
    return _2
  def _conv_forward(self: __torch__.torch.nn.modules.conv.Conv1d,
    input: Tensor,
    weight: Tensor,
    bias: Optional[Tensor]) -> Tensor:
    _3 = torch.conv1d(input, weight, bias, [1], [0], [1])
    return _3
