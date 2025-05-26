class ModuleList(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  __annotations__["0"] = __torch__.wenet.transformer.decoder_layer.DecoderLayer
  __annotations__["1"] = __torch__.wenet.transformer.decoder_layer.DecoderLayer
  __annotations__["2"] = __torch__.wenet.transformer.decoder_layer.DecoderLayer
  def __len__(self: __torch__.torch.nn.modules.container.___torch_mangle_6.ModuleList) -> int:
    return 3
